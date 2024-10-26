#pragma once

#include "Engine/Vector.hpp"

class Ray;

struct BoundingBox
{
	Vector	lower;
	Vector	upper;

	double	intersect(const Ray& ray) const;
};
