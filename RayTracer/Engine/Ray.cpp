#include "Engine/Ray.hpp"

#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Ray::Ray(const Vector& position, const Vector& direction)
	: m_position(position)
	, m_direction(direction)
	, m_directionInverse(1 / m_direction.x(), 1 / m_direction.y(), 1 / m_direction.z())
{
	assert(direction.length() - 1 <= std::numeric_limits<double>::epsilon());
}

Color Ray::trace(const Scene& scene, uint32_t rayDepth) const
{
	if (rayDepth >= scene.maxRayDepth)
		return Palette::kBlack;

	double	closestIntersectionDistance = Object::kNoIntersection;
	Object*	closestObject = nullptr;

	for (const auto& o : scene.objects)
	{
		double intersectionDistance = o->intersect(*this);
		if (intersectionDistance >= closestIntersectionDistance)
			continue;

		closestIntersectionDistance = intersectionDistance;
		closestObject = o.get();
	}

	if (closestIntersectionDistance == Object::kNoIntersection)
		return scene.background;

	const Vector closestCollisionPoint = m_position.add(m_direction.scale(closestIntersectionDistance));
	return closestObject->illuminate(scene, *this, closestCollisionPoint, rayDepth + 1);
}
