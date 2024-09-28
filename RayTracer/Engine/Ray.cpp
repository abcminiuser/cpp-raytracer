#include "Ray.hpp"

#include "Object.hpp"
#include "Scene.hpp"

#include <limits>

namespace
{
	constexpr uint32_t kMaxDepth = 16;
}

Ray::Ray(Vector pos, Vector dir)
	: m_position(std::move(pos))
	, m_direction(dir.unit())
{

}

Color Ray::trace(const Scene& scene, uint32_t rayDepth) const
{
	if (rayDepth > kMaxDepth)
		return Palette::kBlack;

	float	closestIntersectionDistance = Object::kNoIntersection;
	Object*	closestObject = nullptr;

	for (const auto& o : scene.objects)
	{
		float intersectionDistance = o->intersect(*this);
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

Vector Ray::Reflect(const Vector& incident, const Vector& surfaceNormal)
{
	return incident.add(surfaceNormal.scale(-2 * surfaceNormal.dotProduct(incident))).unit();
}
