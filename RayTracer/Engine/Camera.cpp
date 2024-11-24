#include "Engine/Camera.hpp"

#include "Engine/MathUtil.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <cassert>

Camera::Camera()
	: Camera(Transform(), 16.0 / 9.0, MathUtil::DegreesToRadians(90), 1.0, 0.0)
{

}

Camera::Camera(const Transform& transform, double aspectRatio, double verticalFov, double focusDistance, double aperture)
	: m_transform(transform)
	, m_aspectRatio(aspectRatio)
	, m_verticalFov(verticalFov)
	, m_focusDistance(focusDistance)
	, m_aperture(aperture)
{
	update();
}

void Camera::setTransform(const Transform& transform)
{
	m_transform = transform;
	update();
}

void Camera::setAspectRatio(double aspectRatio)
{
	m_aspectRatio = std::max(aspectRatio, 0.0001);
	update();
}

void Camera::setVerticalFov(double verticalFov)
{
	m_verticalFov = std::clamp(verticalFov, MathUtil::DegreesToRadians(1), MathUtil::DegreesToRadians(180));
	update();
}

void Camera::setFocusDistance(double focusDistance)
{
	m_focusDistance = std::max(focusDistance, 0.0001);
	update();
}

void Camera::setAperture(double aperture)
{
	m_aperture = std::max(aperture, 0.0001);
	update();
}

Color Camera::trace(const Scene& scene, double u, double v) const
{
	assert(u == std::clamp(u, 0.0, 1.0));
	assert(v == std::clamp(v, 0.0, 1.0));

	Vector rayOrigin	= StandardVectors::kZero;
	Vector rayDirection	= m_viewportUpperLeftCorner + (m_viewportHorizontalScan * u) + (m_viewportVerticalScan * v);

	if (m_defocusRadius)
	{
		const Vector defocusX	= StandardVectors::kUnitX * (Random::SignedNormal() * m_defocusRadius);
		const Vector defocusY	= StandardVectors::kUnitY.inverted() * (Random::SignedNormal() * m_defocusRadius);
		const Vector defocusXY	= defocusX + defocusY;

		rayOrigin		+= defocusXY;
		rayDirection	-= defocusXY;
	}

	const Ray rayWorldSpace = Ray(m_transform.untransformPosition(rayOrigin), m_transform.untransformDirection(rayDirection).unit());

	return rayWorldSpace.trace(scene, 0);
}

void Camera::update()
{
	m_viewHeight				= 2 * std::tan(m_verticalFov / 2) * m_focusDistance;
	m_viewWidth					= m_viewHeight * m_aspectRatio;

	m_defocusRadius				= m_focusDistance * (m_aperture / 2);

	m_viewportHorizontalScan	= StandardVectors::kUnitX * m_viewWidth * m_focusDistance;
	m_viewportVerticalScan		= StandardVectors::kUnitY.inverted() * m_viewHeight * m_focusDistance;

	m_viewportUpperLeftCorner	= StandardVectors::kZero - (m_viewportHorizontalScan / 2) - (m_viewportVerticalScan / 2) + (StandardVectors::kUnitZ * m_focusDistance);
}
