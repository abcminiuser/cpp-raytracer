#include "Engine/Ray.hpp"

#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <limits>

namespace
{
	constexpr uint32_t kMaxDepth = 16;
}

Ray::Ray(const Vector& position, const Vector& direction)
	: m_position(position)
	, m_direction(direction)
{
	assert(direction.length() - 1 <= std::numeric_limits<double>::epsilon());
}

Color Ray::trace(const Scene& scene, uint32_t rayDepth) const
{
	if (rayDepth >= kMaxDepth)
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
	return closestObject->illuminate(scene, closestCollisionPoint, *this, rayDepth + 1);
}

Ray Ray::reflect(const Vector& position, const Vector& normal) const
{
	return Ray(position, m_direction.subtract(normal.scale(2 * m_direction.dotProduct(normal))).unit());
}
