#include "Engine/Camera.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, 4.0, 9.0 / 4.0)
{

}

Camera::Camera(const Vector& position, const Vector& target, double viewWidth, double viewHeight)
	: m_position(position)
	, m_direction(target.subtract(position).unit())
	, m_orientation(StandardVectors::kUnitY)
	, m_viewWidth(viewWidth)
	, m_viewHeight(viewHeight)
{
	update();
}

Color Camera::trace(const Scene& scene, double x, double y) const
{
	const Vector rayX = m_right.scale(x);
	const Vector rayY = m_down.scale(y);

	return Ray(m_position, m_direction.add(rayX).add(rayY).unit()).trace(scene, scene.maxRayDepth);
}

void Camera::setPosition(const Vector& position)
{
	m_position = position;
	update();
}

void Camera::setDirection(const Vector& direction)
{
	m_direction = direction;
	update();
}

void Camera::setOrientation(const Vector& orientation)
{
	m_orientation = orientation;
	update();
}

void Camera::update()
{
	m_right = m_orientation.crossProduct(m_direction).unit().scale(m_viewWidth / 2);
	m_down = m_right.crossProduct(m_direction).unit().scale(-m_viewHeight / 2).invert();
}
