#include "Engine/Ray.hpp"

#include "Engine/MathUtil.hpp"
#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include <cassert>

Ray::Ray(const Vector& position, const Vector& direction)
	: m_position(position)
	, m_direction(direction)
	, m_directionInverse(StandardVectors::kUnit / m_direction)
{
	assert(direction.isUnit());
}

Color Ray::trace(const Scene& scene, uint32_t rayDepth) const
{
	double	closestIntersectionDistance = Ray::kNoIntersection;
	Object*	closestObject = nullptr;

	for (const auto& object : scene.objects)
	{
		double intersectionDistance = object->intersect(*this);
		if (intersectionDistance >= closestIntersectionDistance)
			continue;

		closestIntersectionDistance = intersectionDistance;
		closestObject = object.get();
	}

	if (closestIntersectionDistance == Ray::kNoIntersection)
	{
		Vector polarDirection = MathUtil::CartesianToPolar(m_direction);

		double u = .5 + polarDirection.x();
		double v = .5 + polarDirection.y();
		return scene.background->sample(u, v);
	}

	return closestObject->illuminate(scene, *this, closestIntersectionDistance, rayDepth + 1);
}
