#include "SphereObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <cmath>
#include <numbers>

SphereObject::SphereObject(const Vector& position, const Material& material, double radius)
	: Object(position, material)
	, m_radius(radius)
	, m_diameter(m_radius * m_radius)
{

}

Object::IntersectionDistances SphereObject::intersectWith(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	double b = 2 * os.dotProduct(ray.direction());
	double c = os.squaredEuclidianDistance() - m_diameter;
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

Vector SphereObject::normalAt(const Vector& position) const
{
	return position.subtract(m_position).unit();
}

Color SphereObject::colorAt(const Scene& scene, const Ray& ray) const
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
