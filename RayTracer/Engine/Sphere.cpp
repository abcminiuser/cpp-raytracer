#include "Sphere.hpp"

#include "Ray.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

Sphere::Sphere(Vector position, float radius, Color color)
	: m_position(std::move(position))
	, m_radius(radius)
	, m_color(color)
{

}

float Sphere::intersect(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	float b = 2 * os.dotProduct(ray.direction());
	float c = os.dotProduct(os) - (m_radius * m_radius);
	float d = (b * b) - 4 * c;

	if (d < 0)
	{
		// No intersection
		return std::numeric_limits<float>::max();
	}
	else if (d == 0)
	{
		// Single intersection (we hit along the edge)
		return -b / 2;
	}
	else
	{
		// Two intersection solution, choose the closet to the ray
		const float dSqrt = std::sqrt(d);

		const auto sol1 = (-b - dSqrt) / 2;
		const auto sol2 = (-b + dSqrt) / 2;

		return std::min(sol1, sol2);
	}
}

Vector Sphere::normalAt(const Vector& position) const
{
	return position.subtract(m_position).unit();
}

Color Sphere::colorAt(const Vector& position) const
{
	return m_color;
}
