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
	uint32_t imgWidth = m_FinalImage->GetWidth();
	uint32_t imgHeight = m_FinalImage->GetHeight();
	
	for (uint32_t y = 0; y < imgHeight; y++)
	{
		const glm::vec3& rayOrigin = camera.GetPosition();

		Ray ray;
		ray.Origin = camera.GetPosition();
		
		for (uint32_t x = 0; x < imgWidth; x++)
		{
			// get the coordinate of the pixel in the image,
			// normalize it to be between [-1, 1]
			// assign pixel value accordingly
			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];
			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

// Assign colour to each pixel based on its coordinates in the image
glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	// (bx^2 + by^2 + bz^2) * t^2 + 2*t*(ax*bx + ay*by + az*bz) + (ax^2 + ay^2 + az^2 - r^2) = 0
	// a = ray origin
	// b = ray direction
	// r = sphere radius
	// t = hit distance of the ray to sphere

	if (scene.Spheres.size() == 0) 
		return glm::vec4(0, 0, 0, 1); // if there are no spheres return black

	// loop through all spheres in the scene,
	// and render the closest sphere hit by each ray
	// if no sphere is hit, return black
	const Sphere* closestSphere = nullptr;
	float hitDist = FLT_MAX;
	for (const Sphere& sphere : scene.Spheres) 
	{
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
		if (closestZ < hitDist)
		{
			hitDist = closestZ;
			closestSphere = &sphere;
		}
	}

	if (closestSphere == nullptr) return glm::vec4(0.0f, 0.0f, 0.0f, 0.1f);

	glm::vec3 origin = ray.Origin - closestSphere->Position;
	glm::vec3 hitPoint = origin + ray.Direction * hitDist;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 sphereCol = closestSphere->Albedo;

	glm::vec3 lightDir(-1.0f, -1.0f, -1.0f);
	lightDir = glm::normalize(lightDir);
	float lightMult = glm::max(glm::dot(normal, -lightDir), 0.0f);

	sphereCol *= lightMult;
	return glm::vec4(sphereCol, 1.0f);
}