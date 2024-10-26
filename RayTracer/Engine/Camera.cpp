#include "Engine/Camera.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

#include <numbers>

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, StandardVectors::kUnitY, 1920, 16.0 / 9.0, std::numbers::pi / 2, 10.0, 0.0)
{

}

Camera::Camera(const Vector& position, const Vector& target, const Vector& orientation, double imageWidth, double aspectRatio, double verticalFov, double focusDistance, double defocusAngle)
	: m_position(position)
	, m_direction((target - position).unit())
	, m_orientation(orientation)
	, m_imageWidth(imageWidth)
	, m_aspectRatio(aspectRatio)
	, m_verticalFov(verticalFov)
	, m_focusDistance(focusDistance)
	, m_defocusAngle(defocusAngle)
{
	update();
}

Color Camera::trace(const Scene& scene, double u, double v) const
{
	const Vector rayX	= m_u * u * (m_viewWidth / 2);
	const Vector rayY	= m_v * v * (m_viewHeight / 2);
	const Vector rayXY	= rayX + rayY;

	Vector rayOrigin	= m_position + (m_w * m_focusDistance);
	Vector rayDirection	= (m_direction * m_focusDistance) + rayXY;

	if (m_defocusRadius)
	{
		const Vector defocusX	= m_u * (Random::SignedNormal() * m_defocusRadius);
		const Vector defocusY	= m_v * (Random::SignedNormal() * m_defocusRadius);
		const Vector defocusXY	= defocusX + defocusY;

		rayOrigin		-= defocusXY;
		rayDirection	+= defocusXY;
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
	m_u	= m_orientation.crossProduct(m_direction).unit();
	m_v	= m_w.crossProduct(m_u).unit();

	const size_t imageHeight = std::max<size_t>(static_cast<size_t>(m_imageWidth / m_aspectRatio), 1);

    m_viewHeight	= 2 * std::tan(m_verticalFov / 2) * m_focusDistance;
    m_viewWidth		= m_viewHeight * (m_imageWidth / imageHeight);

    m_defocusRadius	= m_focusDistance * std::tan(m_defocusAngle / 2);
}
