#include "Engine/Object/SphereObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

SphereObject::SphereObject(const Vector& position, const Vector& rotation, double radius, std::shared_ptr<Texture> texture, const Material& material)
	: Object(position, rotation, std::move(texture), material)
	, m_diameter(radius * radius)
{

}

double SphereObject::intersectWith(const Ray& ray) const
{
	const double b = 2 * ray.position().dotProduct(ray.direction());
	const double c = ray.position().lengthSquared() - m_diameter;
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

void SphereObject::getIntersectionProperties(const Vector& position, Vector& normal, Color& color) const
{
	normal	= position.unit();
	color	= colorAt(position, normal);
}

Color SphereObject::colorAt(const Vector& position, const Vector& normal) const
{
	if (! m_texture)
		return Palette::kBlack;

	double u = .5 + std::atan2(normal.z(), normal.x()) / (2 * std::numbers::pi);
	double v = .5 + std::asin(normal.y()) / std::numbers::pi;

	return m_texture->colorAt(u, v);
}
