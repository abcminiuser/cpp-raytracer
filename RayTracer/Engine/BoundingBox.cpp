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

bool BoundingBox::intersects(const BoundingBox& boundingBox) const
{
	const auto xInBounds = lower.x() <= boundingBox.upper.x() && boundingBox.lower.x() <= upper.x();
	const auto yInBounds = lower.y() <= boundingBox.upper.y() && boundingBox.lower.y() <= upper.y();
	const auto zInBounds = lower.z() <= boundingBox.upper.z() && boundingBox.lower.z() <= upper.z();

	return xInBounds && yInBounds && zInBounds;
}

bool BoundingBox::contains(const Vector& point) const
{
	if (point.x() < lower.x() || point.x() > upper.x())
		return false;

	if (point.y() < lower.y() || point.y() > upper.y())
		return false;

	if (point.z() < lower.z() || point.z() > upper.z())
		return false;

	return true;
}
