#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Camera
{
public:
					Camera();
					Camera(const Vector& position, const Vector& target, const Vector& orientation, double imageWidth, double aspectRatio, double verticalFov, double focusDistance, double aperture);

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

	double			m_imageWidth;
	double			m_aspectRatio;
	double			m_verticalFov;
	double			m_focusDistance;
	double			m_aperture;

	double			m_viewWidth;
	double			m_viewHeight;
	double			m_defocusRadius;

	Vector			m_viewportHorizontalScan;
	Vector			m_viewportVerticalScan;
	Vector			m_viewportUpperLeftCorner;

	Vector			m_u;
	Vector			m_v;
	Vector			m_w;
};
