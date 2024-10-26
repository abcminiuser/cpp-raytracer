#include "Engine/BoundingBox.hpp"

#include "Engine/Ray.hpp"

double BoundingBox::intersect(const Ray& ray) const
{
	const Vector t1 = (lower - ray.position()) * ray.directionInverse();
	const Vector t2 = (upper - ray.position()) * ray.directionInverse();

	const Vector minPoint = VectorUtils::MinPoint(t1, t2);
	const Vector maxPoint = VectorUtils::MaxPoint(t1, t2);

	const double tMin = VectorUtils::MaxComponent(minPoint);
	const double tMax = VectorUtils::MinComponent(maxPoint);

	if (tMax < 0)
	{
		// Intersection is behind us
		return Ray::kNoIntersection;
	}

	if (tMin > tMax)
	{
		// No intersection
		return Ray::kNoIntersection;
	}

	return tMin;
}
