#pragma once

#include "Color.hpp"

#include <array>
#include <limits>

class Ray;

struct Scene;
struct Vector;

class Object
{
public:
	virtual							~Object() = default;

	float							intersect(const Ray& ray) const;
	Color							illuminate(const Scene& scene, const Vector& position) const;

protected:
	using IntersectionDistances = std::array<float, 2>;

	static inline constexpr float kNoIntersection = std::numeric_limits<float>::max();

	virtual IntersectionDistances	intersectWith(const Ray& ray) const = 0;
	virtual Vector					normalAt(const Vector& position) const = 0;
	virtual Color					colorAt(const Scene& scene, const Ray& ray) const = 0;
};
