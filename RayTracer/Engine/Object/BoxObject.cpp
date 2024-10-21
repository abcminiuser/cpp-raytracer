#include "Engine/Object/BoxObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr auto kFrontNormal		= StandardVectors::kUnitZ.inverted();
	constexpr auto kLeftNormal		= StandardVectors::kUnitX.inverted();
	constexpr auto kTopNormal		= StandardVectors::kUnitY.inverted();
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

	const auto t1 = ray.position().inverted() * ray.directionInverse();
	const auto t2 = (m_size - ray.position()) * ray.directionInverse();

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

void BoxObject::getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
{
	if (std::abs(position.z()) < kComparisonThreshold)
	{
		normal		= kFrontNormal; // Front face
		tangent		= StandardVectors::kUnitY;
		bitangent	= StandardVectors::kUnitX;
	}
	else if (std::abs(position.x()) < kComparisonThreshold)
	{
		normal		= kLeftNormal; // Left face
		tangent		= StandardVectors::kUnitY;
		bitangent	= StandardVectors::kUnitZ.inverted();
	}
	else if (std::abs(position.y()) < kComparisonThreshold)
	{
		normal		= kTopNormal; // Top face
		tangent		= StandardVectors::kUnitZ;
		bitangent	= StandardVectors::kUnitX;
	}
	else if (std::abs(position.y() - m_size.y()) < kComparisonThreshold)
	{
		normal		= kBottomNormal; // Bottom face
		tangent		= StandardVectors::kUnitZ.inverted();
		bitangent	= StandardVectors::kUnitX;
	}
	else if (std::abs(position.x() - m_size.x()) < kComparisonThreshold)
	{
		normal		= kRightNormal; // Right face
		tangent		= StandardVectors::kUnitY;
		bitangent	= StandardVectors::kUnitZ;
	}
	else
	{
		normal		= kBackNormal; // Back face
		tangent		= StandardVectors::kUnitY;
		bitangent	= StandardVectors::kUnitX.inverted();
	}

	uv = uvAt(position, normal);
}

Vector BoxObject::uvAt(const Vector& position, const Vector& normal) const
{
	constexpr double kStepU = (1.0 / 4);
	constexpr double kStepV = (1.0 / 3);

	const auto dLower = position / m_size;
	const auto dUpper = (m_size - position) / m_size;

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
