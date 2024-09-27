#include "Sphere.hpp"

#include "Ray.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

Sphere::Sphere(Vector pos, float size, Color color)
	: m_position(std::move(pos))
	, m_size(size)
	, m_color(color)
{

}

float Sphere::intersect(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	float b = 2 * os.dotProduct(ray.direction());
	float c = os.dotProduct(os) - (m_size * m_size);
	float d = (b * b) - 4 * c;

	// If discriminant is less than zero, no intersection
	if (d < 0)
		return std::numeric_limits<float>::max();

	if (d == 0)
		return -b / 2;

	const float dSqrt = std::sqrt(d);

	const auto sol1 = (-b - dSqrt) / 2;
	const auto sol2 = (-b + dSqrt) / 2;

	return std::min(sol1, sol2);
}

Color Sphere::color(const Vector& position) const
{
	return m_color;
}
