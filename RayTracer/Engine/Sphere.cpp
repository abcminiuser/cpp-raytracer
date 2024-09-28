#include "Sphere.hpp"

#include "Material.hpp"
#include "Ray.hpp"

#include <cmath>

Sphere::Sphere(Vector position, Material material, float radius)
	: Object(std::move(position), std::move(material))
	, m_radius(radius)
{

}

Object::IntersectionDistances Sphere::intersectWith(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	float b = 2 * os.dotProduct(ray.direction());
	float c = os.squaredEuclidianDistance() - (m_radius * m_radius);
	float d = (b * b) - 4 * c;

	if (d < 0)
	{
		// No intersection
		return { kNoIntersection, kNoIntersection };
	}
	else if (d == 0)
	{
		// Single intersection (we hit along the edge)
		return { -b / 2, kNoIntersection };
	}
	else
	{
		const float dSqrt = std::sqrtf(d);

		// Two intersection solutions
		return {
			(-b - dSqrt) / 2,
			(-b + dSqrt) / 2
		};
	}
}

Vector Sphere::normalAt(const Vector& position) const
{
	return position.subtract(m_position).unit();
}

Color Sphere::colorAt(const Scene& scene, const Ray& ray) const
{
	return m_material.color;
}
