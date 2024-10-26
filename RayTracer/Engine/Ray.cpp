#include "Engine/Ray.hpp"

#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

Ray::Ray(const Vector& position, const Vector& direction)
	: m_position(position)
	, m_direction(direction)
	, m_directionInverse(Vector(1, 1, 1) / m_direction)
{
	assert(direction.length() - 1 <= std::numeric_limits<double>::epsilon());
}

Color Ray::trace(const Scene& scene, uint32_t rayDepthRemaining) const
{
	if (rayDepthRemaining-- == 0)
		return Palette::kBlack;

	double	closestIntersectionDistance = Ray::kNoIntersection;
	Object*	closestObject = nullptr;

	for (const auto& o : scene.objects)
	{
		double intersectionDistance = o->intersect(*this);
		if (intersectionDistance >= closestIntersectionDistance)
			continue;

		closestIntersectionDistance = intersectionDistance;
		closestObject = o.get();
	}

	if (closestIntersectionDistance == Ray::kNoIntersection)
		return scene.background;

	const Vector closestCollisionPoint = m_position + (m_direction * closestIntersectionDistance);
	return closestObject->illuminate(scene, *this, closestCollisionPoint, rayDepthRemaining);
}
