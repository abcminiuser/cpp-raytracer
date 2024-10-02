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
	, m_directionInverse(1 / m_direction.x(), 1 / m_direction.y(), 1 / m_direction.z())
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

std::optional<Ray> Ray::refract(const Vector& position, const Vector& normal, double n1, double n2) const
{
	// https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf

	double n = n1 / n2;
	double cosI = m_direction.dotProduct(normal);
	double sinT2 = (n * n) * (1.0 - (cosI * cosI));

	if (sinT2 > 1.0)
		return std::nullopt;

	double cosT = std::sqrt(1.0 - sinT2);
	return Ray(position, m_direction.scale(n).add(normal.scale(n * cosI - cosT)).unit());
}
