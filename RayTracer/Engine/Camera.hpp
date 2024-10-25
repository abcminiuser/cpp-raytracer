#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Camera
{
public:
					Camera();
					Camera(const Vector& position, const Vector& target, const Vector& orientation, double viewWidth, double viewHeight);

	const Vector&	position() const	{ return m_position; }
	const Vector&	direction() const	{ return m_direction; }
	const Vector&	orientation() const	{ return m_orientation; }

	const Vector&	u() const			{ return m_u; }
	const Vector&	v() const			{ return m_v; }
	const Vector&	w() const			{ return m_w; }

	void			setPosition(const Vector& position);
	void			setDirection(const Vector& direction);
	void			setOrientation(const Vector& orientation);

	Color			trace(const Scene& scene, double u, double v) const;

private:
	void			update();

private:
	Vector			m_position;
	Vector			m_direction;
	Vector			m_orientation;

	double			m_viewWidth;
	double			m_viewHeight;

	Vector			m_u;
	Vector			m_v;
	Vector			m_w;
};
