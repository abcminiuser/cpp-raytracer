#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Camera
{
public:
			Camera();
			Camera(Vector position, Vector target, double lensWidth, double lensHeight);

	Color	trace(const Scene& scene, double x, double y) const;

private:
	Vector	m_position;
	Vector	m_direction;

	double	m_lensWidth;
	double	m_lensHeight;

	Vector	m_right;
	Vector	m_down;
};
