#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Ray
{
public:
					Ray(const Vector& pos, const Vector& dir);

	const Vector&	position() const	{ return m_position; }
	const Vector&	direction() const	{ return m_direction; }

	Color			trace(const Scene& scene, uint32_t rayDepth) const;

private:
	Vector			m_position;
	Vector			m_direction;
};
