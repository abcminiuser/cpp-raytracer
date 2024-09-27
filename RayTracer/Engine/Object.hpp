#pragma once

#include "Color.hpp"

class Ray;
struct Vector;

class Object
{
public:
	virtual			~Object() = default;

public:
	virtual float	intersect(const Ray& ray) const = 0;
	virtual Color	color(const Vector& position) const = 0;
};
