#include "Sphere.hpp"

#include "Material.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <cmath>
#include <numbers>

Sphere::Sphere(Vector position, Material material, double radius)
	: Object(std::move(position), std::move(material))
	, m_radius(radius)
{

}

Object::IntersectionDistances Sphere::intersectWith(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	double b = 2 * os.dotProduct(ray.direction());
	double c = os.squaredEuclidianDistance() - (m_radius * m_radius);
	double d = (b * b) - 4 * c;

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
		const double dSqrt = std::sqrt(d);

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
	if (! m_material.texture)
		return Palette::kBlack;

	const auto x = ray.direction().x();
	const auto y = ray.direction().y();
	const auto z = ray.direction().z();

	double u = .5 + std::atan2(z, x) / (2 * std::numbers::pi);
	double v = .5 + std::asin(y) / std::numbers::pi;

	return m_material.texture->colorAt(u, v);
}
