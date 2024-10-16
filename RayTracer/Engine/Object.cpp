#include "Engine/Object.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Object::Object(const Vector& position, const Vector& rotation, std::shared_ptr<Material> material)
	: m_material(std::move(material))
	, m_position(position)
	, m_hasRotation(rotation != Vector())
	, m_rotationMatrix(MatrixUtils::RotationMatrix(rotation))
	, m_rotationMatrixInverse(MatrixUtils::RotationMatrix(rotation.invert()))
{
	if (! m_material)
		throw std::runtime_error("Object created with no associated material");
}

double Object::intersect(const Ray& ray) const
{
	const Ray rayObjectSpace = Ray(rotate(ray.position().subtract(m_position), m_rotationMatrix), rotate(ray.direction(), m_rotationMatrix).unit());

	const double closestIntersectionDistance = intersectWith(rayObjectSpace);
	if (closestIntersectionDistance < kComparisonThreshold)
		return kNoIntersection;

	return closestIntersectionDistance;
}

Color Object::illuminate(const Scene& scene, const Ray& ray, const Vector& position, uint32_t rayDepthRemaining) const
{
	const Vector positionObjectSpace = rotate(position.subtract(m_position), m_rotationMatrix);

	Vector	normalObjectSpace;
	Vector	uv;
	getIntersectionProperties(positionObjectSpace, normalObjectSpace, uv);
	assert(normalObjectSpace.length() - 1 <= std::numeric_limits<double>::epsilon());

	const Vector normalWorldSpace = rotate(normalObjectSpace, m_rotationMatrixInverse).unit();

	return m_material->illuminate(scene, ray, position, normalWorldSpace, uv, rayDepthRemaining);
}

Vector Object::rotate(const Vector& vector, const Matrix<3, 3>& rotation) const
{
	return m_hasRotation ? rotation.multiply(vector).toVector() : vector;
}
