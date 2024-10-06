#pragma once

#include "Engine/Color.hpp"
#include "Engine/Material.hpp"
#include "Engine/Vector.hpp"

#include <limits>
#include <stdint.h>

class Ray;

struct Scene;

class Object
{
public:
	static inline constexpr double kNoIntersection			= std::numeric_limits<double>::max();
	static inline constexpr double kComparisonThreshold		= std::numeric_limits<double>::epsilon() * 1000;

					Object(const Vector& position, const Material& material);
	virtual			~Object() = default;

	double			intersect(const Ray& ray) const;
	Color			illuminate(const Scene& scene, const Vector& position, const Ray& ray, uint32_t rayDepth) const;

protected:
	virtual double	intersectWith(const Ray& ray) const = 0;
	virtual Vector	normalAt(const Vector& position) const = 0;
	virtual Color	colorAt(const Scene& scene, const Ray& ray) const = 0;

protected:
	Vector			m_position;
	Material		m_material;
};
