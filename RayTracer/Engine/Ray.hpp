#pragma once

#include "Color.hpp"
#include "Vector.hpp"

struct Scene;

class Ray
{
public:
					Ray(Vector pos, Vector dir);

	const Vector&	position() const
	{
		return m_position;
	}

	const Vector&	direction() const
	{
		return m_direction;
	}

	Color			trace(const Scene& scene) const;

private:
	Vector	m_position;
	Vector	m_direction;
};
