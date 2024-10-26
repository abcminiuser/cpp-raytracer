#pragma once

#include "Engine/Vector.hpp"

class Ray;

struct BoundingBox
{
	Vector	lower;
	Vector	upper;

public:
	double	intersect(const Ray& ray) const;
	bool	intersects(const BoundingBox& boundingBox) const;
	bool	contains(const Vector& point) const;
};
