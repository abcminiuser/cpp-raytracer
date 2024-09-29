#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Camera
{
public:
			Camera();
			Camera(const Vector& position, const Vector& target, double viewWidth, double viewHeight);

	Color	trace(const Scene& scene, double x, double y) const;

private:
	Vector	m_position;
	Vector	m_direction;

	double	m_viewWidth;
	double	m_viewHeight;

	Vector	m_right;
	Vector	m_down;
};
