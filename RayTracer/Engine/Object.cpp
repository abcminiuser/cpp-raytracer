#include "Object.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

#include <algorithm>

namespace
{
	constexpr double kMinIntersectionDistance = 0.0000001;
	constexpr double kSpecularMultiplier = 32;
}

Object::Object(const Vector& position, const Material& material)
	: m_position(position)
	, m_material(material)
{

}

double Object::intersect(const Ray& ray) const
{
	IntersectionDistances distances = intersectWith(ray);

	if (distances[0] < kMinIntersectionDistance)
		distances[0] = kNoIntersection;

	if (distances[1] < kMinIntersectionDistance)
		distances[1] = kNoIntersection;

	return std::min(distances[0], distances[1]);
}

Color Object::illuminate(const Scene& scene, const Vector& position, uint32_t rayDepth) const
{
	const Vector	normal			= normalAt(position);
	const Color		objectColor		= colorAt(scene, Ray(position, normal));
	const Vector	unitReflection 	= position.reflect(normal).unit();

	Color finalColor = objectColor.scale(m_material.ambient);

	if (m_material.reflectivity > 0)
	{
		const auto reflectionColor = Ray(position, unitReflection).trace(scene, rayDepth);
		const Color lightingColor = reflectionColor.scale(m_material.reflectivity);
		finalColor = finalColor.add(lightingColor);
	}

	for (const auto& l : scene.lights)
	{
		const Vector objectToLight = l->position().subtract(position);

		bool shadowed = std::any_of(
			scene.objects.begin(), scene.objects.end(),
			[&](const auto& o)
			{
				const double intersectionDistance = o->intersect(Ray(position, objectToLight));
				return intersectionDistance <= objectToLight.length();
			});
		if (shadowed)
			continue;

		if (m_material.diffuse > 0)
		{
			double brightness = normal.dotProduct(objectToLight.unit());
			if (brightness > 0)
			{
				const Color lightingColor = l->illuminate(objectColor, brightness * m_material.diffuse);
				finalColor = finalColor.add(lightingColor);
			}
		}

		if (m_material.specular > 0)
		{
			double brightness = unitReflection.dotProduct(objectToLight.unit());
			if (brightness > 0)
			{
				brightness *= std::pow(brightness, kSpecularMultiplier * m_material.specular * m_material.specular);

				const Color lightingColor = l->illuminate(objectColor, brightness * m_material.specular);
				finalColor = finalColor.add(lightingColor);
			}
		}
	}

	return finalColor;
}
