#include "Engine/Ray.hpp"

#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include <limits>

namespace
{
	constexpr uint32_t kMaxDepth = 16;
}

Ray::Ray(const Vector& pos, const Vector& dir)
	: m_position(pos)
	, m_direction(dir.unit())
{

}

Color Ray::trace(const Scene& scene, uint32_t rayDepth) const
{
	if (rayDepth > kMaxDepth)
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

	if (! closestObject)
		return scene.background;

	const Vector closestCollisionPoint = m_position.add(m_direction.scale(closestIntersectionDistance));
	return closestObject->illuminate(scene, closestCollisionPoint, rayDepth + 1);
}
