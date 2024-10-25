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
	const Vector ray00U = m_u * - .5 * (m_viewWidth / 2);
	const Vector ray00V = m_v * - .5 * (m_viewHeight / 2);
	const Vector ray00 = ray00U + ray00V;

	const Vector rayX = m_u * u * (m_viewWidth / 2);
	const Vector rayY = m_v * v * (m_viewHeight / 2);
	const Vector rayXY = rayX + rayY;

	const Vector rayDirection = (m_direction + ray00 + rayXY);

	return Ray(m_position, rayDirection.unit()).trace(scene, scene.maxRayDepth);
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
