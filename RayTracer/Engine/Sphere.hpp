#pragma once

#include "Color.hpp"
#include "Object.hpp"
#include "Vector.hpp"

class Sphere
	: public Object
{
public:
			Sphere(Vector pos, float size, Color color);
			~Sphere() = default;

// Object i/f:
public:
	float	intersect(const Ray& ray) const override;
	Color	color(const Vector& position) const;

private:
	Vector	m_position;
	float	m_size;
	Color	m_color;
};
