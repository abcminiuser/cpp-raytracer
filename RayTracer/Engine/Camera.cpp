#include "Engine/Camera.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, 4.0, 9.0 / 4.0)
{

}

Camera::Camera(const Vector& position, const Vector& target, double viewWidth, double viewHeight)
	: m_viewWidth(viewWidth)
	, m_viewHeight(viewHeight)
{
	setPosition(position);
	setDirection(target.subtract(position).unit());
}

Color Camera::trace(const Scene& scene, double x, double y) const
{
	const Vector rayX = m_right.scale(x);
	const Vector rayY = m_down.scale(y);

	return Ray(m_position, m_direction.add(rayX).add(rayY).unit()).trace(scene, 0);
}

void Camera::setPosition(const Vector& position)
{
	m_position = position;
}

void Camera::setDirection(const Vector& direction)
{
	m_direction = direction;

	m_right = StandardVectors::kUnitY.crossProduct(m_direction).unit().scale(m_viewWidth / 2);
	m_down = m_right.crossProduct(m_direction).unit().scale(-m_viewHeight / 2).invert();
}
