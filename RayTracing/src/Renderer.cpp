#include "Renderer.h"

#include "Walnut/Random.h"

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

void Renderer::Render()
{
	uint32_t imgWidth = m_FinalImage->GetWidth();
	uint32_t imgHeight = m_FinalImage->GetHeight();
	
	for (uint32_t y = 0; y < imgHeight; y++)
	{
		for (uint32_t x = 0; x < imgWidth; x++)
		{
			// get the coordinate of the pixel in the image,
			// normalize it to be between [-1, 1]
			// assign pixel value accordingly
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // normalize between [-1, 1] and not [0, 1]
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

// Assign colour to each pixel based on its coordinates in the image
uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	// we have a sphere centered at the origin with radius 0.5f
	// and the camera is located at (0, 0, 2) facing the sphere
	float radius = 0.5f;
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDir(coord.x, coord.y, -1.0f);
	// rayDir = glm::normalize(rayDir) // make it a unit vector

	// (bx^2 + by^2 + bz^2) * t^2 + 2*t*(ax*bx + ay*by + az*bz) + (ax^2 + ay^2 + az^2 - r^2) = 0
	// a = ray origin
	// b = ray direction
	// r = sphere radius
	// t = hit distance of the ray to sphere

	// here, the floats a, b, and c are the coeffs
	// of the quadratic equation for the sphere,
	// not to be confused with ax, ay, bx, by
	float a = glm::dot(rayDir, rayDir);                         // (bx^2 + by^2 + bz^2)
	float b = 2.0f * glm::dot(rayOrigin, rayDir);               // 2*(ax*bx + ay*by)
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius; // (ax^2 + ay^2 + az^2 - r^2)

	// discriminant = b^2 - 4*a*c
	float disc = b * b - 4 * a * c;

	if (disc >= 0)
		return 0xffff00ff;

	return 0xff000000;
}