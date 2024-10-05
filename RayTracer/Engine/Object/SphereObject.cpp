#include "Engine/Object/SphereObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

SphereObject::SphereObject(const Vector& position, double radius, std::shared_ptr<Texture> texture, const Material& material)
	: Object(position, material)
	, m_diameter(radius * radius)
	, m_texture(std::move(texture))
{

}

double SphereObject::intersectWith(const Ray& ray) const
{
	const auto os = ray.position().subtract(m_position);

	const double b = 2 * os.dotProduct(ray.direction());
	const double c = os.squaredEuclidianDistance() - m_diameter;
	const double d = (b * b) - 4 * c;

	if (d < 0)
	{
		// No intersection
		return kNoIntersection;
	}
	else if (d == 0)
	{
		// Single intersection (we hit along the edge)
		return -b / 2;
	}
	else
	{
		// Two intersection solutions (both sides of the sphere)

		const double dSqrt = std::sqrt(d);

		const double solution1 = (-b - dSqrt) / 2;
		const double solution2 = (-b + dSqrt) / 2;

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
