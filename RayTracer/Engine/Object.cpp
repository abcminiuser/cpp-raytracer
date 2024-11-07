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
	: m_boundingBox(boundingBox)
	, m_transform(transform)
	, m_material(material ? std::move(material) : kDefaultMaterial)
{
	// Transform the bounding box into world space, as that's how we'll use it for fast
	// intersection pre-checks.
	m_boundingBox = m_transform.untransformBoundingBox(m_boundingBox);
}

double Object::intersect(const Ray& ray) const
{
	// Test for intersection in the world-space bounding box first, so we can potentially
	// skip a bunch of work.
	if (m_boundingBox.intersect(ray) == Ray::kNoIntersection)
		return Ray::kNoIntersection;

	const Ray 		rayObjectSpace		= m_transform.transformRay(ray);

	const double closestIntersectionDistance = intersectWith(rayObjectSpace);
	if (closestIntersectionDistance < kComparisonThreshold)
		return Ray::kNoIntersection;

	const Vector	positionObjectSpace	= rayObjectSpace.at(closestIntersectionDistance);
	const Vector	positionWorldSpace	= m_transform.untransformPosition(positionObjectSpace);
	const double	distanceWorldSpace	= (positionWorldSpace - ray.position()).length();

	return distanceWorldSpace;
}

Color Object::illuminate(const Scene& scene, const Ray& ray, double distance, uint32_t rayDepthRemaining) const
{
	const Vector	directionObjectSpace	= m_transform.transformDirection(ray.direction());

	const Vector	positionWorldSpace		= ray.at(distance);
	const Vector	positionObjectSpace 	= m_transform.transformPosition(positionWorldSpace);

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

	const Vector 	normalWorldSpace		= m_transform.untransformDirection(normalObjectSpace);

	assert(normalWorldSpace.isUnit());

	return m_material->illuminate(scene, ray, positionWorldSpace, normalWorldSpace, uv, rayDepthRemaining);
}
