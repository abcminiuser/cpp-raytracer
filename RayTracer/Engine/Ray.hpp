#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Ray
{
public:
						Ray(const Vector& position, const Vector& direction);

	const Vector&		position() const			{ return m_position; }
	const Vector&		direction() const			{ return m_direction; }
	const Vector&		directionInverse() const	{ return m_directionInverse; }

	Color				trace(const Scene& scene, uint32_t rayDepthRemaining) const;

private:
	Vector				m_position;
	Vector				m_direction;
	Vector				m_directionInverse;
};
