#include "BoxObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <cmath>

BoxObject::BoxObject(const Vector& position, const Vector& size, std::shared_ptr<Texture> texture, const Material& material)
	: Object(position, material)
	, m_size(size)
	, m_texture(std::move(texture))
{
	const auto position2 = position.add(size);

	m_lowerCorner = Vector(
		std::min(position.x(), position2.x()),
		std::min(position.y(), position2.y()),
		std::min(position.z(), position2.z())
	);

	m_upperCorner = Vector(
		std::max(position.x(), position2.x()),
		std::max(position.y(), position2.y()),
		std::max(position.z(), position2.z())
	);
}

double BoxObject::intersectWith(const Ray& ray) const
{
	// https://en.wikipedia.org/wiki/Slab_method

	const auto t1 = m_lowerCorner.subtract(ray.position()).multiply(ray.directionInverse());
	const auto t2 = m_upperCorner.subtract(ray.position()).multiply(ray.directionInverse());

	double tMin = kNoIntersection;
	double tMax = kNoIntersection;

	if (ray.direction().x() == 0)
	{
		// Perpendicular to X axis and won't ever intersect there (so ignore it).
		tMin =
			std::max(
				std::min(t1.y(), t2.y()),
				std::min(t1.z(), t2.z())
			);
		tMax =
			std::min(
				std::max(t1.y(), t2.y()),
				std::min(t1.z(), t2.z())
			);
	}
	else if (ray.direction().y() == 0)
	{
		// Perpendicular to Y axis and won't ever intersect there (so ignore it).
		tMin =
			std::max(
				std::min(t1.x(), t2.x()),
				std::min(t1.z(), t2.z())
			);
		tMax =
			std::min(
				std::max(t1.x(), t2.x()),
				std::min(t1.z(), t2.z())
			);
	}
	else if (ray.direction().z() == 0)
	{
		// Perpendicular to Z axis and won't ever intersect there (so ignore it).
		tMin =
			std::max(
				std::min(t1.x(), t2.x()),
				std::min(t1.y(), t2.y())
			);
		tMax =
			std::min(
				std::max(t1.x(), t2.x()),
				std::min(t1.y(), t2.y())
			);
	}
	else
	{
		tMin =
			std::max({
				std::min(t1.x(), t2.x()),
				std::min(t1.y(), t2.y()),
				std::min(t1.z(), t2.z())
			});
		tMax =
			std::min({
				std::max(t1.x(), t2.x()),
				std::max(t1.y(), t2.y()),
				std::max(t1.z(), t2.z())
			});
	}

	if (tMax < 0)
	{
		// Intersection is behind us, no intersection
		return kNoIntersection;
	}

	if (tMin >= tMax)
	{
		// No intersection
		return kNoIntersection;
	}

	if (tMin < kComparisonThreshold)
		tMin = kNoIntersection;

	return tMin;
}

Vector BoxObject::normalAt(const Vector& position) const
{
	if (std::abs(position.z() - m_lowerCorner.z()) < kComparisonThreshold)
		return StandardVectors::kUnitZ.invert(); // Front face
	else if (std::abs(position.x() - m_lowerCorner.x()) < kComparisonThreshold)
		return StandardVectors::kUnitX.invert(); // Left face
	else if (std::abs(position.y() - m_lowerCorner.y()) < kComparisonThreshold)
		return StandardVectors::kUnitY.invert(); // Bottom face
	else if (std::abs(position.y() - m_upperCorner.y()) < kComparisonThreshold)
		return StandardVectors::kUnitY; // Top face
	else if (std::abs(position.x() - m_upperCorner.x()) < kComparisonThreshold)
		return StandardVectors::kUnitX; // Right face
	else
		return StandardVectors::kUnitZ; // Rear face
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

	if (ray.direction() == StandardVectors::kUnitZ.invert())
	{
		// Front face
		u = uStep * (1 + dLower.x());
		v = vStep * (1 + dLower.y());
	}
	else if (ray.direction() == StandardVectors::kUnitX.invert())
	{
		// Left face
		u = uStep * (0 + dUpper.z());
		v = vStep * (1 + dLower.y());
	}
	else if (ray.direction() == StandardVectors::kUnitY)
	{
		// Bottom face
		u = uStep * (1 + dLower.x());
		v = vStep * (2 + dLower.z());
	}
	else if (ray.direction() == StandardVectors::kUnitY.invert())
	{
		// Top face
		u = uStep * (1 + dUpper.z());
		v = vStep * (0 + dLower.x());
	}
	else if (ray.direction() == StandardVectors::kUnitX)
	{
		// Right face
		u = uStep * (2 + dLower.z());
		v = vStep * (1 + dLower.y());
	}
	else if (ray.direction() == StandardVectors::kUnitZ)
	{
		// Rear face
		u = uStep * (3 + dUpper.x());
		v = vStep * (1 + dLower.y());
	}

	return m_texture->colorAt(u, v);
}
