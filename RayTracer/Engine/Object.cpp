#include "Object.hpp"

#include "Vector.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

#include <algorithm>

namespace
{
	constexpr auto kMinIntersectionDistance = 0.0001f;
}

float Object::intersect(const Ray& ray) const
{
	IntersectionDistances distances = intersectWith(ray);

	if (distances[0] < kMinIntersectionDistance)
		distances[0] = kNoIntersection;

	if (distances[1] < kMinIntersectionDistance)
		distances[1] = kNoIntersection;

	return std::min(distances[0], distances[1]);
}

Color Object::illuminate(const Scene& scene, const Vector& position) const
{
	const Vector	normal		= normalAt(position);
	const Color		objectColor	= colorAt(scene, Ray(position, normal));

	Color finalColor = Palette::kBlack;

	for (const auto& l : scene.lights)
	{
		const Vector objectToLight = l->position().subtract(position);

		bool shadowed = std::any_of(
			scene.objects.begin(), scene.objects.end(),
			[&](const auto& o)
			{
				const float intersectionDistance = o->intersect(Ray(position, objectToLight));
				return intersectionDistance <= objectToLight.length();
			});
		if (shadowed)
			continue;

		const float brightness = normal.dotProduct(objectToLight.unit());
		if (brightness > 0)
			finalColor = finalColor.add(l->illuminate(objectColor, position, brightness));
	}

	return finalColor;
}
