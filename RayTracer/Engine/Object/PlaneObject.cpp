#include "Engine/Object/PlaneObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

namespace
{
	constexpr Vector kNormal 	= StandardVectors::kUnitY;
	constexpr Vector kTangent	= StandardVectors::kUnitX;
	constexpr Vector kBitangent	= StandardVectors::kUnitZ;
}

PlaneObject::PlaneObject(const Transform& transform, std::shared_ptr<Material> material)
	: Object(BoundingBox(StandardVectors::kMin, StandardVectors::kMax), transform, std::move(material))
{

}

double PlaneObject::intersectWith(const Ray& ray) const
{
	const auto angle = ray.direction().dotProduct(kNormal);

	if (angle == 0)
	{
		// No intersection
		return Ray::kNoIntersection;
	}

	// Intersection at a single point (as we're infinitely thin)
	const auto b = kNormal.dotProduct(ray.position());
	return -b / angle;
}

void PlaneObject::getIntersectionProperties(const Vector& direction, const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
{
	normal		= kNormal;
	tangent		= kTangent;
	bitangent	= kBitangent;
	uv			= uvAt(position, normal);
}

Vector PlaneObject::uvAt(const Vector& position, const Vector& normal) const
{
	auto u = kTangent.dotProduct(position);
	auto v = kBitangent.dotProduct(position);

	return Vector(u, v, 0);
}
