#include "BoxObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr Vector MinPoint(const Vector& v1, const Vector& v2)
	{
		return Vector(
			std::min(v1.x(), v2.x()),
			std::min(v1.y(), v2.y()),
			std::min(v1.z(), v2.z())
		);
	}

	constexpr Vector MaxPoint(const Vector& v1, const Vector& v2)
	{
		return Vector(
			std::max(v1.x(), v2.x()),
			std::max(v1.y(), v2.y()),
			std::max(v1.z(), v2.z())
		);
	}

	constexpr auto kFrontNormal = StandardVectors::kUnitZ.invert();
	constexpr auto kLeftNormal = StandardVectors::kUnitX.invert();
	constexpr auto kTopNormal = StandardVectors::kUnitY.invert();
	constexpr auto kBottomNormal = StandardVectors::kUnitY;
	constexpr auto kRightNormal = StandardVectors::kUnitX;
	constexpr auto kBackNormal = StandardVectors::kUnitZ;
}

BoxObject::BoxObject(const Vector& position, const Vector& size, std::shared_ptr<Texture> texture, const Material& material)
	: Object(position, material)
	, m_size(size)
	, m_texture(std::move(texture))
	, m_lowerCorner(MinPoint(position, position.add(size)))
	, m_upperCorner(MaxPoint(position, position.add(size)))
{

}

double BoxObject::intersectWith(const Ray& ray) const
{
	// https://en.wikipedia.org/wiki/Slab_method

	const auto t1 = m_lowerCorner.subtract(ray.position()).multiply(ray.directionInverse());
	const auto t2 = m_upperCorner.subtract(ray.position()).multiply(ray.directionInverse());

	const Vector maxPoint = MaxPoint(t1, t2);
	const double tMax = std::min({ maxPoint.x(), maxPoint.y(), maxPoint.z() });

	if (tMax < 0)
	{
		// Intersection is behind us, no intersection
		return kNoIntersection;
	}

	const Vector minPoint = MinPoint(t1, t2);
	const double tMin = std::max({ minPoint.x(), minPoint.y(), minPoint.z() });

	if (tMin >= tMax)
	{
		// No intersection
		return kNoIntersection;
	}

	// Intersection with a face
	return tMin;
}

Vector BoxObject::normalAt(const Vector& position) const
{
	if (std::abs(position.z() - m_lowerCorner.z()) < kComparisonThreshold)
		return kFrontNormal; // Front face
	else if (std::abs(position.x() - m_lowerCorner.x()) < kComparisonThreshold)
		return kLeftNormal; // Left face
	else if (std::abs(position.y() - m_lowerCorner.y()) < kComparisonThreshold)
		return kTopNormal; // Top face
	else if (std::abs(position.y() - m_upperCorner.y()) < kComparisonThreshold)
		return kBottomNormal; // Bottom face
	else if (std::abs(position.x() - m_upperCorner.x()) < kComparisonThreshold)
		return kRightNormal; // Right face
	else
		return kBackNormal; // Back face
}

Color BoxObject::colorAt(const Scene& scene, const Ray& ray) const
{
	if (! m_texture)
		return Palette::kBlack;

	constexpr double uStep = (1.0 / 4);
	constexpr double vStep = (1.0 / 3);

	const auto dLower = ray.position().subtract(m_lowerCorner).divide(m_size);
	const auto dUpper = m_upperCorner.subtract(ray.position()).divide(m_size);

	double u = 0;
	double v = 0;

	if (ray.direction() == kFrontNormal)
	{
		// Front face
		u = uStep * (1 + dLower.x());
		v = vStep * (1 + dLower.y());
	}
	else if (ray.direction() == kLeftNormal)
	{
		// Left face
		u = uStep * (0 + dUpper.z());
		v = vStep * (1 + dLower.y());
	}
	else if (ray.direction() == kTopNormal)
	{
		// Top face
		u = uStep * (1 + dUpper.z());
		v = vStep * (0 + dLower.x());
	}
	else if (ray.direction() == kBottomNormal)
	{
		// Bottom face
		u = uStep * (1 + dLower.x());
		v = vStep * (2 + dLower.z());
	}
	else if (ray.direction() == kRightNormal)
	{
		// Right face
		u = uStep * (2 + dLower.z());
		v = vStep * (1 + dLower.y());
	}
	else if (ray.direction() == kBackNormal)
	{
		// Back face
		u = uStep * (3 + dUpper.x());
		v = vStep * (1 + dLower.y());
	}

	return m_texture->colorAt(u, v);
}
