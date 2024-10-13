#include "Engine/Object.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Object::Object(const Vector& position, const Vector& rotation, std::shared_ptr<Texture> texture, const Material& material)
	: m_position(position)
	, m_rotation(rotation)
	, m_texture(std::move(texture))
	, m_material(material)
	, m_rotationMatrix(MatrixUtils::RotationMatrix(m_rotation))
	, m_rotationMatrixInverse(MatrixUtils::RotationMatrix(m_rotation.invert()))
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

	Vector	normal;
	Color	objectColor;
	getIntersectionProperties(positionObjectSpace, normal, objectColor);
	assert(normal.length() - 1 <= std::numeric_limits<double>::epsilon());

	normal = rotate(normal, m_rotationMatrixInverse).unit();

	const Ray reflectionRay	= ray.reflect(position, normal);

	Color finalColor = objectColor.scale(m_material.ambient);

	if (scene.allowReflections && m_material.reflectivity > 0)
	{
		const auto reflectionColor = reflectionRay.trace(scene, rayDepth);
		finalColor = finalColor.add(reflectionColor.scale(m_material.reflectivity));
	}

	if (scene.allowRefractions && m_material.refractivity > 0)
	{
		const auto refractionRay = ray.refract(position, normal, scene.airRefractionIndex, m_material.refractionIndex);

		if (refractionRay)
		{
			const auto refractionColor = refractionRay->trace(scene, rayDepth);
			finalColor = finalColor.add(refractionColor.scale(m_material.refractivity));
		}
	}

	if (scene.allowLighting)
	{
		for (const auto& l : scene.lights)
			finalColor = finalColor.add(l->illuminate(scene, normal, objectColor, reflectionRay, m_material));
	}

	return finalColor;
}

Vector Object::rotate(const Vector& vector, const Matrix<3, 3>& rotation) const
{
	Matrix<3, 1> targetMatrix;
	targetMatrix(0, 0) = vector.x();
	targetMatrix(1, 0) = vector.y();
	targetMatrix(2, 0) = vector.z();

	const auto result = rotation.multiply(targetMatrix);

	return Vector(result(0, 0), result(0, 1), result(0, 2));
}
