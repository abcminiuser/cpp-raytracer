#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

#include <limits>

struct Scene;

class Ray
{
public:
	static inline constexpr double kNoIntersection = std::numeric_limits<double>::max();

						Ray(const Vector& position, const Vector& direction);

	const Vector&		position() const			{ return m_position; }
	const Vector&		direction() const			{ return m_direction; }
	const Vector&		directionInverse() const	{ return m_directionInverse; }

	const Vector		at(double distance) const	{ return m_position + (m_direction * distance); }

	Color				trace(const Scene& scene, uint32_t rayDepth) const;

private:
	Vector				m_position;
	Vector				m_direction;
	Vector				m_directionInverse;
};
