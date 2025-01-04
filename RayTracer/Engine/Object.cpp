#include "Engine/Object.hpp"

#include "Engine/Material.hpp"
#include "Engine/Material/DiffuseMaterial.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>

namespace
{
	const auto kDefaultMaterial = std::make_shared<DiffuseMaterial>(nullptr, nullptr);
}

Object::Object(const BoundingBox& boundingBox, const Transform& transform, std::shared_ptr<Material> material)
	: m_boundingBox(transform.untransformBoundingBox(boundingBox)) // Transform the bounding box into world space, as that's how we'll use it for fast intersection pre-checks.
	, m_transform(transform)
	, m_material(material ? std::move(material) : kDefaultMaterial)
{

}

double Object::intersect(const Ray& ray) const
{
	// Test for intersection in the world-space bounding box first, so we can potentially
	// skip a bunch of work.
	if (m_boundingBox.intersect(ray) == Ray::kNoIntersection)
		return Ray::kNoIntersection;

	// We can avoid an expensive transform back to world space, if we *don't* normalize our
	// direction here. A non-normalized transformed ray will give an intersection distance
	// that is valid for both the world-space and object-space rays.
	const Ray 		rayObjectSpaceUnnormalized	= Ray(m_transform.transformPosition(ray.position()), m_transform.transformDirection(ray.direction()));

	const double distance = intersectWith(rayObjectSpaceUnnormalized);
	if (distance < kComparisonThreshold)
		return Ray::kNoIntersection;

	return distance;
}

Color Object::illuminate(const Scene& scene, const Ray& ray, const Vector& position, uint32_t rayDepth) const
{
	const Vector	directionObjectSpace		= m_transform.transformDirection(ray.direction()).unit();
	const Vector	positionObjectSpace 		= m_transform.transformPosition(position);

	Vector normalObjectSpace;
	Vector tangent;
	Vector bitangent;
	Vector uv;
	getIntersectionProperties(directionObjectSpace, positionObjectSpace, normalObjectSpace, tangent, bitangent, uv);

	assert(normalObjectSpace.isUnit());
	assert(tangent.isUnit());
	assert(bitangent.isUnit());

	// Remap the object's normal using the texture normal map if present
	normalObjectSpace = m_material->mapNormal(normalObjectSpace, tangent, bitangent, uv);

	assert(normalObjectSpace.isUnit());

	const Vector 	normalWorldSpace			= m_transform.untransformDirection(normalObjectSpace).unit();

	assert(normalWorldSpace.isUnit());

	return m_material->illuminate(scene, ray, position, normalWorldSpace, uv, rayDepth);
}
