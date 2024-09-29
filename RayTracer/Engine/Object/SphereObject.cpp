#include "SphereObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

SphereObject::SphereObject(const Vector& position, double radius, std::shared_ptr<Texture> texture, const Material& material)
	: Object(position, material)
	, m_texture(std::move(texture))
	, m_radius(radius)
	, m_diameter(m_radius * m_radius)
{

}

double SphereObject::intersectWith(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	double b = 2 * os.dotProduct(ray.direction());
	double c = os.squaredEuclidianDistance() - m_diameter;
	double d = (b * b) - 4 * c;

	if (d < 0)
	{
		// No intersection
		return kNoIntersection;
	}
	else if (d == 0)
	{
		// Single intersection (we hit along the edge)

		auto solution = -b / 2;
		if (solution < kMinIntersectionDistance)
			solution = kNoIntersection;

		return solution;
	}
	else
	{
		// Two intersection solutions (both sides of the sphere)

		const double dSqrt = std::sqrt(d);

		auto solution1 = (-b - dSqrt) / 2;
		if (solution1 < kMinIntersectionDistance)
			solution1 = kNoIntersection;

		auto solution2 = (-b + dSqrt) / 2;
		if (solution2 < kMinIntersectionDistance)
			solution2 = kNoIntersection;

		return std::min(solution1, solution2);
	}
}

Vector SphereObject::normalAt(const Vector& position) const
{
	return position.subtract(m_position).unit();
}

Color SphereObject::colorAt(const Scene& scene, const Ray& ray) const
{
	if (! m_texture)
		return Palette::kBlack;

	const auto x = ray.direction().x();
	const auto y = ray.direction().y();
	const auto z = ray.direction().z();

	double u = .5 + std::atan2(z, x) / (2 * std::numbers::pi);
	double v = .5 + std::asin(y) / std::numbers::pi;

	return m_texture->colorAt(u, v);
}
