#include "Engine/Camera.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, StandardVectors::kUnitY, 4.0, 9.0 / 4.0)
{

}

Camera::Camera(const Vector& position, const Vector& target, const Vector& orientation, double viewWidth, double viewHeight)
	: m_position(position)
	, m_direction((target - position).unit())
	, m_orientation(orientation)
	, m_viewWidth(viewWidth)
	, m_viewHeight(viewHeight)
{
	update();
}

Color Camera::trace(const Scene& scene, double u, double v) const
{
	const Vector rayX = m_u * (u - .5) * (m_viewWidth / 2);
	const Vector rayY = m_v * (v - .5) * (m_viewHeight / 2);

	const Vector rayDirection = (m_direction + rayX + rayY).unit();

	return Ray(m_position, rayDirection).trace(scene, scene.maxRayDepth);
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
}
