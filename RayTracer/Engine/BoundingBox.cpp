#include "Engine/BoundingBox.hpp"

#include "Engine/Ray.hpp"

double BoundingBox::intersect(const Ray& ray) const
{
	const Vector t1 = (m_lower - ray.position()) * ray.directionInverse();
	const Vector t2 = (m_upper - ray.position()) * ray.directionInverse();

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
	const auto xInBounds = m_lower.x() <= boundingBox.m_upper.x() && boundingBox.m_lower.x() <= m_upper.x();
	const auto yInBounds = m_lower.y() <= boundingBox.m_upper.y() && boundingBox.m_lower.y() <= m_upper.y();
	const auto zInBounds = m_lower.z() <= boundingBox.m_upper.z() && boundingBox.m_lower.z() <= m_upper.z();

	return xInBounds && yInBounds && zInBounds;
}

bool BoundingBox::contains(const Vector& point) const
{
	if (point.x() < m_lower.x() || point.x() > m_upper.x())
		return false;

	if (point.y() < m_lower.y() || point.y() > m_upper.y())
		return false;

	if (point.z() < m_lower.z() || point.z() > m_upper.z())
		return false;

	return true;
}
