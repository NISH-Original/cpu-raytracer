#include <cmath>

#include "Renderer.h"

#include "Walnut/Random.h"

namespace Utils 
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage) 
	{
		// if no resize necessary, return
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) return;
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	
	uint32_t imgWidth = m_FinalImage->GetWidth();
	uint32_t imgHeight = m_FinalImage->GetHeight();
	
	for (uint32_t y = 0; y < imgHeight; y++)
	{
		const glm::vec3& rayOrigin = camera.GetPosition();
		
		for (uint32_t x = 0; x < imgWidth; x++)
		{
			glm::vec4 color = PerPixel(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

// function that runs for each pixel
// returns color that needs to be rendered for said pixel
glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	glm::vec3 color(0.0f);

	float multiplier = 1.0f;
	
	// bounce the light across surfaces n number of times
	// reduce the intensity of light by a multiplier per bounce
	int bounces = 2;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyColor * multiplier;
			break;
		}

		// add shading on the surface as per the angle of the light source
		glm::vec3 lightDir(-1.0f, -1.0f, -1.0f);
		lightDir = glm::normalize(lightDir);
		float lightMult = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f);

		const Sphere& closestSphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& closestMaterial = m_ActiveScene->Materials[closestSphere.MaterialIndex];
		glm::vec3 sphereCol = closestMaterial.Albedo;
		sphereCol *= lightMult;
		color += sphereCol * multiplier;

		multiplier *= 0.5f;

		// added a little bias of 0.0001 so that the reflection image
		// on the sphere surface does not clip with the sphere itself
		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + closestMaterial.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(color, 1.0f);
}

// run ClosestHit function if hit a sphere
// and Miss if otherwise
Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	// (bx^2 + by^2 + bz^2) * t^2 + 2*t*(ax*bx + ay*by + az*bz) + (ax^2 + ay^2 + az^2 - r^2) = 0
	// a = ray origin
	// b = ray direction
	// r = sphere radius
	// t = hit distance of the ray to sphere

	// loop through all spheres in the scene,
	// and render the closest sphere hit by each ray
	// if no sphere is hit, return black
	int closestSphere = -1;
	float hitDist = FLT_MAX;
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) 
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		
		glm::vec3 origin = ray.Origin - sphere.Position;

		// here, the floats a, b, and c are the coeffs
		// of the quadratic equation for the sphere,
		// not to be confused with ax, ay, bx, by

		float a = glm::dot(ray.Direction, ray.Direction);     // (bx^2 + by^2 + bz^2)
		float b = 2.0f * glm::dot(origin, ray.Direction);     // 2*(ax*bx + ay*by)
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius; // (ax^2 + ay^2 + az^2 - r^2)

		// discriminant = b^2 - 4*a*c
		float disc = b * b - 4 * a * c;

		if (disc < 0.0f)
			continue;

		// float z = (-b + sqrt(disc)) / (2.0f * a);

		float closestZ = (-b - sqrt(disc)) / (2.0f * a);
		if (closestZ > 0.0f && closestZ < hitDist)
		{
			hitDist = closestZ;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0) 
		return Miss(ray);

	return ClosestHit(ray, hitDist, closestSphere);
}

// return payload of ray that hit the closest sphere
Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;
	
	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];
	
	glm::vec3 origin = ray.Origin - closestSphere.Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;	

	return payload;
}

// return the payload of ray that missed all spheres in scene
Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}