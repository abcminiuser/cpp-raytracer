#pragma once

#include "Color.hpp"
#include "Vector.hpp"
#include "Scene.hpp"

class Ray
{
public:
			Ray(Vector pos, Vector dir);

	Color	trace(const Scene& scene) const;

private:
	Vector	m_position;
	Vector	m_direction;
};
