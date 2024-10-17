#include "Engine/Object/BoxObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr auto kFrontNormal		= StandardVectors::kUnitZ.invert();
	constexpr auto kLeftNormal		= StandardVectors::kUnitX.invert();
	constexpr auto kTopNormal		= StandardVectors::kUnitY.invert();
	constexpr auto kBottomNormal	= StandardVectors::kUnitY;
	constexpr auto kRightNormal		= StandardVectors::kUnitX;
	constexpr auto kBackNormal		= StandardVectors::kUnitZ;
}

BoxObject::BoxObject(const Vector& position, const Vector& rotation, std::shared_ptr<Material> material, const Vector& size)
	: Object(position, rotation, std::move(material))
	, m_size(size)
{

}

double BoxObject::intersectWith(const Ray& ray) const
{
	// https://en.wikipedia.org/wiki/Slab_method

	const auto t1 = Vector(0, 0, 0).subtract(ray.position()).multiply(ray.directionInverse());
	const auto t2 = m_size.subtract(ray.position()).multiply(ray.directionInverse());

	const Vector minPoint = VectorUtils::MinPoint(t1, t2);
	const Vector maxPoint = VectorUtils::MaxPoint(t1, t2);

	const double tMin = VectorUtils::MaxComponent(minPoint);
	const double tMax = VectorUtils::MinComponent(maxPoint);

	if (tMax < 0)
	{
		// Intersection is behind us
		return kNoIntersection;
	}

	if (tMin > tMax)
	{
		// No intersection
		return kNoIntersection;
	}

	// Intersection with a face
	return tMin;
}

void BoxObject::getIntersectionProperties(const Vector& position, Vector& normal, Vector& uv) const
{
	normal	= normalAt(position);
	uv		= uvAt(position, normal);
}

Vector BoxObject::normalAt(const Vector& position) const
{
	if (std::abs(position.z()) < kComparisonThreshold)
		return kFrontNormal; // Front face
	else if (std::abs(position.x()) < kComparisonThreshold)
		return kLeftNormal; // Left face
	else if (std::abs(position.y()) < kComparisonThreshold)
		return kTopNormal; // Top face
	else if (std::abs(position.y() - m_size.y()) < kComparisonThreshold)
		return kBottomNormal; // Bottom face
	else if (std::abs(position.x() - m_size.x()) < kComparisonThreshold)
		return kRightNormal; // Right face
	else
		return kBackNormal; // Back face
}

Vector BoxObject::uvAt(const Vector& position, const Vector& normal) const
{
	constexpr double kStepU = (1.0 / 4);
	constexpr double kStepV = (1.0 / 3);

	const auto dLower = position.divide(m_size);
	const auto dUpper = m_size.subtract(position).divide(m_size);

	double u = 0;
	double v = 0;

	if (normal == kFrontNormal)
	{
		u = kStepU * (1 + dLower.x());
		v = kStepV * (1 + dLower.y());
	}
	else if (normal == kLeftNormal)
	{
		u = kStepU * (0 + dUpper.z());
		v = kStepV * (1 + dLower.y());
	}
	else if (normal == kTopNormal)
	{
		u = kStepU * (1 + dUpper.z());
		v = kStepV * (0 + dLower.x());
	}
	else if (normal == kBottomNormal)
	{
		u = kStepU * (1 + dLower.x());
		v = kStepV * (2 + dLower.z());
	}
	else if (normal == kRightNormal)
	{
		u = kStepU * (2 + dLower.z());
		v = kStepV * (1 + dLower.y());
	}
	else if (normal == kBackNormal)
	{
		u = kStepU * (3 + dUpper.x());
		v = kStepV * (1 + dLower.y());
	}

	return Vector(u, v, 0);
}
