#pragma once

#include "Engine/Color.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Vector.hpp"

struct Scene;

class Camera
{
public:
						Camera();
						Camera(const Transform& transform, double aspectRatio, double verticalFov, double focusDistance, double aperture);

	const Transform&	transform() const		{ return m_transform; }
	double				aspectRatio() const		{ return m_aspectRatio; }
	double				verticalFov() const		{ return m_verticalFov; }
	double				focusDistance() const	{ return m_focusDistance; }
	double				aperture() const		{ return m_aperture; }

	void				setTransform(const Transform& transform);
	void				setAspectRatio(double aspectRatio);
	void				setVerticalFov(double verticalFov);
	void				setFocusDistance(double focusDistance);
	void				setAperture(double aperture);

	Color				trace(const Scene& scene, double u, double v) const;

private:
	void				update();

private:
	Transform			m_transform;

	double				m_aspectRatio;
	double				m_verticalFov;
	double				m_focusDistance;
	double				m_aperture;

	double				m_viewWidth;
	double				m_viewHeight;
	double				m_defocusRadius;

	Vector				m_viewportHorizontalScan;
	Vector				m_viewportVerticalScan;
	Vector				m_viewportUpperLeftCorner;
};
