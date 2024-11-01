#include "Engine/Object.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Object::Object(const Vector& position, const Vector& rotation, const Vector& scale, std::shared_ptr<Material> material)
	: m_material(std::move(material))
	, m_position(position)
	, m_rotation(rotation)
	, m_scale(scale)
{
	if (! m_material)
		throw std::runtime_error("Object created with no associated material");

	m_transform			= MatrixUtils::RotateMatrix(m_rotation.inverted()) * MatrixUtils::ScaleMatrix(StandardVectors::kUnit / m_scale) * MatrixUtils::TranslateMatrix(m_position.inverted());
	m_inverseTransform	= (MatrixUtils::TranslateMatrix(m_position) * MatrixUtils::RotateMatrix(m_rotation.inverted()) * MatrixUtils::ScaleMatrix(m_scale)).transposed();
}

double Object::intersect(const Ray& ray) const
{
	const Ray rayObjectSpace = MatrixUtils::Transform(ray, m_transform);

	const double closestIntersectionDistance = intersectWith(rayObjectSpace);
	if (closestIntersectionDistance < kComparisonThreshold)
		return Ray::kNoIntersection;

	return closestIntersectionDistance;
}

Color Object::illuminate(const Scene& scene, const Ray& ray, double distance, uint32_t rayDepthRemaining) const
{
	const Ray		rayObjectSpace		= MatrixUtils::Transform(ray, m_transform);
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

	const Vector 	normalWorldSpace	= MatrixUtils::Transform(normalObjectSpace, m_inverseTransform, true).unit();
	const Vector 	positionWorldSpace	= ray.at(distance);

	return m_material->illuminate(scene, ray, positionWorldSpace, normalWorldSpace, uv, rayDepthRemaining);
}
