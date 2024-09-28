#pragma once

#include "Color.hpp"
#include "Material.hpp"
#include "Vector.hpp"

#include <array>
#include <limits>
#include <stdint.h>

class Ray;

struct Scene;

class Object
{
public:
	static inline constexpr float kNoIntersection = std::numeric_limits<float>::max();

									Object(Vector position, Material material);
	virtual							~Object() = default;

	float							intersect(const Ray& ray) const;
	Color							illuminate(const Scene& scene, const Vector& position, uint32_t rayDepth) const;

protected:
	using IntersectionDistances = std::array<float, 2>;

	virtual IntersectionDistances	intersectWith(const Ray& ray) const = 0;
	virtual Vector					normalAt(const Vector& position) const = 0;
	virtual Color					colorAt(const Scene& scene, const Ray& ray) const = 0;

protected:
	Vector							m_position;
	Material						m_material;
};
