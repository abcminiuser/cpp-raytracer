#include "Object.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <cassert>
#include <limits>

Object::Object(const Vector& position, const Material& material)
	: m_position(position)
	, m_material(material)
{

}

double Object::intersect(const Ray& ray) const
{
	const auto closestIntersectionDistance = intersectWith(ray);
	assert(closestIntersectionDistance > kComparisonThreshold);

	return closestIntersectionDistance;
}

Color Object::illuminate(const Scene& scene, const Vector& position, const Ray& ray, uint32_t rayDepth) const
{
	const Vector	normal			= normalAt(position);
	const Color		objectColor		= colorAt(scene, Ray(position, normal));
	const Ray		reflectionRay	= ray.reflect(position, normal);

	assert(normal.length() - 1 <= std::numeric_limits<double>::epsilon());

	Color finalColor = objectColor.scale(m_material.ambient);

	if (m_material.reflectivity > 0)
	{
		const auto reflectionColor = reflectionRay.trace(scene, rayDepth);
		finalColor = finalColor.add(reflectionColor.scale(m_material.reflectivity));
	}

	if (m_material.refractivity > 0)
	{
		const auto refractionRay = ray.refract(position, normal, scene.airRefractionIndex, m_material.refractionIndex);

		if (refractionRay)
		{
			const auto refractionColor = refractionRay->trace(scene, rayDepth);
			finalColor = finalColor.add(refractionColor.scale(m_material.refractivity));
		}
	}

	for (const auto& l : scene.lights)
		finalColor = finalColor.add(l->illuminate(scene, normal, objectColor, reflectionRay, m_material));

	return finalColor;
}
