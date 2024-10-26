#pragma once

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

struct BoundingBox
{
	static constexpr auto kNoIntersection = Ray::kNoIntersection;

	Vector	lower;
	Vector	upper;

	double	intersect(const Ray& ray) const;
};
