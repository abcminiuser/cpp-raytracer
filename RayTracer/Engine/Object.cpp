#include "Engine/Object.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Object::Object(const Vector& position, const Vector& rotation, std::shared_ptr<Texture> texture, std::shared_ptr<Material> material)
	: m_texture(std::move(texture))
	, m_material(std::move(material))
	, m_position(position)
	, m_hasRotation(rotation != Vector())
	, m_rotationMatrix(MatrixUtils::RotationMatrix(rotation))
	, m_rotationMatrixInverse(MatrixUtils::RotationMatrix(rotation.invert()))
{

}

double Object::intersect(const Ray& ray) const
{
	const Ray rayObjectSpace = Ray(rotate(ray.position().subtract(m_position), m_rotationMatrix), rotate(ray.direction(), m_rotationMatrix).unit());

	const auto closestIntersectionDistance = intersectWith(rayObjectSpace);
	if (closestIntersectionDistance < kComparisonThreshold)
		return kNoIntersection;

	return closestIntersectionDistance;
}

Color Object::illuminate(const Scene& scene, const Vector& position, const Ray& ray, uint32_t rayDepth) const
{
	const Vector positionObjectSpace = rotate(position.subtract(m_position), m_rotationMatrix);

	Vector	normalObjectSpace;
	Color	objectColor;
	getIntersectionProperties(positionObjectSpace, normalObjectSpace, objectColor);
	assert(normalObjectSpace.length() - 1 <= std::numeric_limits<double>::epsilon());

	if (! scene.lighting)
		return objectColor;

	const auto normal = rotate(normalObjectSpace, m_rotationMatrixInverse).unit();

	Ray		scatterRay			= m_material->scatter(ray, position, normal);
	Color	collectedLightColor = scatterRay.trace(scene, rayDepth + 1);
	return objectColor.multiply(collectedLightColor);
}

Vector Object::rotate(const Vector& vector, const Matrix<3, 3>& rotation) const
{
	return m_hasRotation ? rotation.multiply(vector).toVector() : vector;
}
