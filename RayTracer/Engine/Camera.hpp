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

	const Transform&	transform() const { return m_transform; }

	void				setTransform(const Transform& transform);

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
