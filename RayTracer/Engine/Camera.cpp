#include "Camera.hpp"

#include "Ray.hpp"
#include "Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, 4.0f, 9.0f/4.0f)
{}

Camera::Camera(Vector position, Vector target, float lensWidth, float lensHeight)
	: m_position(std::move(position))
	, m_direction(target.subtract(position).unit())
	, m_lensWidth(lensWidth)
	, m_lensHeight(lensHeight)
	, m_right(StandardVectors::kUnitY.crossProduct(m_direction).unit().scale(m_lensWidth / 2))
	, m_up(m_right.crossProduct(m_direction).unit().scale(-m_lensHeight / 2))
{}

Color Camera::trace(const Scene& scene, float x, float y) const
{
	const Vector rayX = m_right.scale(x);
	const Vector rayY = m_up.scale(y).invert();

	return Ray(m_position, m_direction.add(rayX).add(rayY)).trace(scene);
}
