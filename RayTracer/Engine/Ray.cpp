#include "Engine/Ray.hpp"

#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include <cassert>
#include <cmath>
#include <numbers>

Ray::Ray(const Vector& position, const Vector& direction)
	: m_position(position)
	, m_direction(direction)
	, m_directionInverse(StandardVectors::kUnit / m_direction)
{
	assert(direction.isUnit());
}

Color Ray::trace(const Scene& scene, uint32_t rayDepthRemaining) const
{
	if (rayDepthRemaining-- == 0)
		return Palette::kBlack;

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
		double u = .5 + std::atan2(m_direction.z(), m_direction.x()) / (2 * std::numbers::pi);
		double v = .5 + std::asin(m_direction.y()) / std::numbers::pi;

		return scene.background->sample(u, v);
	}

	return closestObject->illuminate(scene, *this, closestIntersectionDistance, rayDepthRemaining);
}
