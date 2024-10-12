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
	, m_rotationMatrix(makeRotationMatrix(m_rotation))
	, m_rotationMatrixInverse(makeRotationMatrix(m_rotation.invert()))
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

Matrix Object::makeRotationMatrix(const Vector& rotation) const
{
	const auto& alpha = rotation.x();
	const auto cosAlpha = std::cos(alpha);
	const auto sinAlpha = std::sin(alpha);

	const auto& beta = rotation.y();
	const auto cosBeta = std::cos(beta);
	const auto sinBeta = std::sin(beta);

	const auto& gamma = rotation.z();
	const auto cosGamma = std::cos(gamma);
	const auto sinGamma = std::sin(gamma);

	Matrix rotationMatrix(3, 3);
	rotationMatrix(0, 0) = cosAlpha * cosBeta;
	rotationMatrix(0, 1) = (cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma);
	rotationMatrix(0, 2) = (cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma);
	rotationMatrix(1, 0) = sinAlpha * cosBeta;
	rotationMatrix(1, 1) = (sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma);
	rotationMatrix(1, 2) = (sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma);
	rotationMatrix(2, 0) = -sinBeta;
	rotationMatrix(2, 1) = cosBeta * sinGamma;
	rotationMatrix(2, 2) = cosBeta * cosGamma;

	return rotationMatrix;
}

Vector Object::rotate(const Vector& vector, const Matrix& rotation) const
{
	Matrix targetMatrix(3, 1);
	targetMatrix(0, 0) = vector.x();
	targetMatrix(1, 0) = vector.y();
	targetMatrix(2, 0) = vector.z();

	const Matrix result = rotation.multiply(targetMatrix);

	return Vector(result(0, 0), result(0, 1), result(0, 2));
}
