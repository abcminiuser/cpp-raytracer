#include "Engine/Ray.hpp"

#include "Engine/MathUtil.hpp"
#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

Ray::Ray(const Vector& position, const Vector& direction)
	: m_position(position)
	, m_direction(direction)
	, m_directionInverse(StandardVectors::kUnit / m_direction)
{

}

Color Ray::trace(const Scene& scene, uint32_t rayDepth) const
{
	double			closestIntersectionDistance = Ray::kNoIntersection;
	const Object*	closestObject = nullptr;

	// Find out what the closest interested object is, ans its distance to us.
	for (const auto& object : scene.objects)
	{
		// First do a check against the object's bounding box; if we don't hit that
		// or hit it further away than our current closest object, we can skip the
		// expensive proper intersection test below.
		if (object->boundingBox().intersect(*this) >= closestIntersectionDistance)
			continue;

		// Do the full object intersection test, to see exactly where our ray hits
		// the object.
		double intersectionDistance = object->intersect(*this);
		if (intersectionDistance >= closestIntersectionDistance)
			continue;

		closestIntersectionDistance = intersectionDistance;
		closestObject = object.get();
	}

	if (! closestObject)
	{
		// We hit nothing, texture based on the scene background instead.

		Vector polarDirection = MathUtil::CartesianToPolar(m_direction);

		double u = .5 + polarDirection.x();
		double v = .5 + polarDirection.y();
		return scene.background->sample(u, v);
	}

	// Texture based on the intersected object.
	const auto intersectionPoint = at(closestIntersectionDistance);
	return closestObject->illuminate(scene, *this, intersectionPoint, rayDepth + 1);
}
