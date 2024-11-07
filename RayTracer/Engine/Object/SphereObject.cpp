#include "Engine/Object/SphereObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace
{
	constexpr auto kBoundingBox =
		BoundingBox(
			StandardVectors::kUnit.inverted() / 2,
			StandardVectors::kUnit / 2
		);
}

SphereObject::SphereObject(const Transform& transform, std::shared_ptr<Material> material)
	: Object(kBoundingBox, transform, std::move(material))
{

}

double SphereObject::intersectWith(const Ray& ray) const
{
	const double b = 2 * ray.position().dotProduct(ray.direction());
	const double c = ray.position().lengthSquared() - .25;
	const double d = (b * b) - 4 * c;

	if (d < 0)
	{
		// No intersection
		return Ray::kNoIntersection;
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

void SphereObject::getIntersectionProperties(const Vector& direction, const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
{
	normal		= position.unit();
	tangent		= normal.crossProduct(direction).unit();
	bitangent	= tangent.crossProduct(normal).unit();
	uv			= uvAt(position, normal);
}

Vector SphereObject::uvAt(const Vector& position, const Vector& normal) const
{
	double u = .5 + std::atan2(normal.z(), normal.x()) / (2 * std::numbers::pi);
	double v = .5 + std::asin(normal.y()) / std::numbers::pi;

	return Vector(u, v, 0);
}
