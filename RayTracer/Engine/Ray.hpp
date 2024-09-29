#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Ray
{
public:
					Ray(const Vector& position, const Vector& direction);

	const Vector&	position() const	{ return m_position; }
	const Vector&	direction() const	{ return m_direction; }

	Color			trace(const Scene& scene, uint32_t rayDepth) const;
	Ray				reflect(const Vector& position, const Vector& normal) const;

private:
	Vector			m_position;
	Vector			m_direction;
};
