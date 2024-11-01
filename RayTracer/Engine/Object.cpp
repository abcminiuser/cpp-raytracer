#include "Engine/Object.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Object::Object(const Transform& transform, std::shared_ptr<Material> material)
	: m_transform(transform)
	, m_material(std::move(material))
{
	if (! m_material)
		throw std::runtime_error("Object created with no associated material");
}

double Object::intersect(const Ray& ray) const
{
	const Ray rayObjectSpace = m_transform.transformRay(ray);

	const double closestIntersectionDistance = intersectWith(rayObjectSpace);
	if (closestIntersectionDistance < kComparisonThreshold)
		return Ray::kNoIntersection;

	return closestIntersectionDistance;
}

Color Object::illuminate(const Scene& scene, const Ray& ray, double distance, uint32_t rayDepthRemaining) const
{
	const Ray		rayObjectSpace		= m_transform.transformRay(ray);
	const Vector	positionObjectSpace	= rayObjectSpace.at(distance);

	Vector normalObjectSpace;
	Vector tangent;
	Vector bitangent;
	Vector uv;
	getIntersectionProperties(positionObjectSpace, normalObjectSpace, tangent, bitangent, uv);

	assert(normalObjectSpace.length() - 1 <= std::numeric_limits<double>::epsilon());
	assert(tangent.length() - 1 <= std::numeric_limits<double>::epsilon());
	assert(bitangent.length() - 1 <= std::numeric_limits<double>::epsilon());

	// Remap the object's normal using the texture normal map if present
	normalObjectSpace = m_material->mapNormal(normalObjectSpace, tangent, bitangent, uv);

	const Vector 	normalWorldSpace	= m_transform.untransformDirection(normalObjectSpace);
	const Vector 	positionWorldSpace	= ray.at(distance);

	return m_material->illuminate(scene, ray, positionWorldSpace, normalWorldSpace, uv, rayDepthRemaining);
}
