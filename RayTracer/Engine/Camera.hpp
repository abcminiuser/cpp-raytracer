#pragma once

#include "Color.hpp"
#include "Vector.hpp"

class Scene;

class Camera
{
public:
			Camera();
			Camera(Vector pos, Vector dir, float lensWidth, float lensHeight);

	Color	trace(const Scene& scene, size_t screenX, size_t screenY) const;

private:
	Vector	m_position;
	Vector	m_direction;

	float	m_lensWidth;
	float	m_lensHeight;

	Vector	m_right;
	Vector	m_up;
};
