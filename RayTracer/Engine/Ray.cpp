#include "Ray.hpp"

#include "Object.hpp"
#include "Scene.hpp"

#include <limits>

namespace
{
	constexpr auto kMinCollisionDistance = 0.00001;
}

Ray::Ray(Vector pos, Vector dir)
	: m_position(std::move(pos))
	, m_direction(dir.unit())
{}

Color Ray::trace(const Scene& scene) const
{
	float	closestIntersectionDistance = std::numeric_limits<float>::max();
	Object*	closestObject = nullptr;

	for (const auto& o : scene.objects)
	{
		float intersectionDistance = o->intersect(*this);
		if (intersectionDistance < kMinCollisionDistance)
			continue;

		if (intersectionDistance > closestIntersectionDistance)
			continue;

		closestIntersectionDistance = intersectionDistance;
		closestObject = o.get();
	}

	if (closestIntersectionDistance == std::numeric_limits<float>::max())
		return scene.background;

	const auto closestCollisionPoint = m_position.add(m_direction.scale(closestIntersectionDistance));
	return closestObject->illuminate(scene, closestCollisionPoint);
}
