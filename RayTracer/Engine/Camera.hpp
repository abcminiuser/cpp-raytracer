#pragma once

#include "Color.hpp"
#include "Vector.hpp"

struct Scene;

class Camera
{
public:
			Camera();
			Camera(Vector position, Vector target, float lensWidth, float lensHeight);

	Color	trace(const Scene& scene, float x, float y) const;

private:
	Vector	m_position;
	Vector	m_direction;

	float	m_lensWidth;
	float	m_lensHeight;

	Vector	m_right;
	Vector	m_down;
};
