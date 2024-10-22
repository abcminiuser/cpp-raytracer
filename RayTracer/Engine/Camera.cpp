#include "Engine/Camera.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, 4.0, 9.0 / 4.0)
{

}

Camera::Camera(const Vector& position, const Vector& target, double viewWidth, double viewHeight)
	: m_position(position)
	, m_direction((target - position).unit())
	, m_orientation(StandardVectors::kUnitY)
	, m_viewWidth(viewWidth)
	, m_viewHeight(viewHeight)
{
	update();
}

Color Camera::trace(const Scene& scene, double x, double y) const
{
	const Vector rayX = m_right * x;
	const Vector rayY = m_down * y;

	const Vector rayDirection = (m_direction + rayX + rayY).unit();

	Color accumulatedSamples;

	for (size_t sample = 0; sample < scene.samplesPerPixel; sample++)
		accumulatedSamples += Ray(m_position, rayDirection).trace(scene, scene.maxRayDepth);

	return accumulatedSamples / scene.samplesPerPixel;
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
	m_right	= m_orientation.crossProduct(m_direction).unit() * (m_viewWidth / 2);
	m_down	= (m_right.crossProduct(m_direction).unit() * (-m_viewHeight / 2)).inverted();
}
