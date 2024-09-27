#pragma once

#include "Color.hpp"

class Ray;

struct Scene;
struct Vector;

class Object
{
public:
	virtual			~Object() = default;

	Color			illuminate(const Scene& scene, const Vector& position);

public:
	virtual float	intersect(const Ray& ray) const = 0;

protected:
	virtual Vector	normalAt(const Vector& position) const = 0;
	virtual Color	colorAt(const Vector& position) const = 0;
};
