#include "Engine/Camera.hpp"

#include "Engine/MathUtil.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, StandardVectors::kUnitY, 16.0 / 9.0, MathUtil::DegreesToRadians(90), 1.0, 0.0)
{

}

Camera::Camera(const Vector& position, const Vector& target, const Vector& orientation, double aspectRatio, double verticalFov, double focusDistance, double aperture)
	: m_position(position)
	, m_direction((target - position).unit())
	, m_orientation(orientation)
	, m_aspectRatio(aspectRatio)
	, m_verticalFov(verticalFov)
	, m_focusDistance(focusDistance)
	, m_aperture(aperture)
{
	update();
}

Color Camera::trace(const Scene& scene, double u, double v) const
{
	Vector rayOrigin	= m_position;
	Vector rayDirection	= m_viewportUpperLeftCorner + (m_viewportHorizontalScan * u) + (m_viewportVerticalScan * v) - m_position;

	if (m_defocusRadius)
	{
		const Vector defocusX	= m_u * (Random::SignedNormal() * m_defocusRadius);
		const Vector defocusY	= m_v * (Random::SignedNormal() * m_defocusRadius);
		const Vector defocusXY	= defocusX + defocusY;

		rayOrigin		+= defocusXY;
		rayDirection	-= defocusXY;
	}

	return Ray(rayOrigin, rayDirection.unit()).trace(scene, scene.maxRayDepth);
}

void Camera::setPosition(const Vector& position)
{
	m_position = position;
	update();
}

void Camera::setDirection(const Vector& direction)
{
	m_direction = direction.unit();
	update();
}

void Camera::setOrientation(const Vector& orientation)
{
	m_orientation = orientation.unit();
	update();
}

void Camera::update()
{
	m_w = m_direction.inverted();
	m_u	= m_orientation.crossProduct(m_direction);
	m_v	= m_w.crossProduct(m_u);

	m_viewHeight				= 2 * std::tan(m_verticalFov / 2) * m_focusDistance;
	m_viewWidth					= m_viewHeight * m_aspectRatio;

	m_defocusRadius				= m_focusDistance * (m_aperture / 2);

	m_viewportHorizontalScan	= m_u * m_viewWidth * m_focusDistance;
	m_viewportVerticalScan		= m_v * m_viewHeight * m_focusDistance;

	m_viewportUpperLeftCorner	= m_position - (m_viewportHorizontalScan / 2) - (m_viewportVerticalScan / 2) - (m_w * m_focusDistance);
}
