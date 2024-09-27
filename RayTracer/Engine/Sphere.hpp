#pragma once

#include "Color.hpp"
#include "Object.hpp"
#include "Vector.hpp"

class Sphere
	: public Object
{
public:
			Sphere(Vector position, float radius, Color color);
			~Sphere() = default;

// Object i/f:
public:
	float	intersect(const Ray& ray) const override;
protected:
	Vector	normalAt(const Vector& position) const override;
	Color	colorAt(const Vector& position) const override;

private:
	Vector	m_position;
	float	m_radius;
	Color	m_color;
};
