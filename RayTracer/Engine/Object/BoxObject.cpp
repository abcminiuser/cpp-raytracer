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

	constexpr auto kBoundingBox		=
		BoundingBox(
			StandardVectors::kZero,
			StandardVectors::kUnit
		);
}

BoxObject::BoxObject(const Transform& transform, std::shared_ptr<Material> material)
	: Object(kBoundingBox, transform, std::move(material))
{

}

double BoxObject::intersectWith(const Ray& ray) const
{
	return BoundingBox(StandardVectors::kZero, StandardVectors::kUnit).intersect(ray);
}

void BoxObject::getIntersectionProperties(const Vector& direction, const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
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
	else if (std::abs(position.y() - 1) < kComparisonThreshold)
	{
		normal		= kBottomNormal; // Bottom face
		tangent		= StandardVectors::kUnitZ.inverted();
		bitangent	= StandardVectors::kUnitX;
	}
	else if (std::abs(position.x() - 1) < kComparisonThreshold)
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

	const auto dLower = position;
	const auto dUpper = StandardVectors::kUnit - position;

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
