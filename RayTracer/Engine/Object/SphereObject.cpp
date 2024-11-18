#include "Engine/Object/SphereObject.hpp"

#include "Engine/MathUtil.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>

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
   	const auto oc = StandardVectors::kZero - ray.position();
    const auto a = ray.direction().lengthSquared();
    const auto h = ray.direction().dotProduct(oc);
    const auto c = oc.lengthSquared() - .25;
	const double d = h*h - a*c;

	if (d < 0)
	{
		// No intersection
		return Ray::kNoIntersection;
	}

	const double dSqrt = std::sqrt(d);

	double solution1 = (h - dSqrt) / a;
	double solution2 = (h + dSqrt) / a;

	if (solution1 < kComparisonThreshold)
		solution1 = Ray::kNoIntersection;

	if (solution2 < kComparisonThreshold)
		solution2 = Ray::kNoIntersection;

	return std::min(solution1, solution2);
}

void SphereObject::getIntersectionProperties(const Vector& direction, const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
{
	normal		= position.unit();
	tangent		= normal.crossProduct(direction).unit();
	bitangent	= tangent.crossProduct(normal);
	uv			= uvAt(position, normal);
}

Vector SphereObject::uvAt(const Vector& position, const Vector& normal) const
{
	Vector polarNormal = MathUtil::CartesianToPolar(normal);

	double u = .5 + polarNormal.x();
	double v = .5 + polarNormal.y();

	return Vector(u, v, 0);
}
