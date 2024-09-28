#include "Engine/Camera.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, StandardVectors::kUnitZ, 4.0, 9.0 / 4.0)
{

}

Camera::Camera(Vector position, Vector target, double viewWidth, double viewHeight)
	: m_position(std::move(position))
	, m_direction(target.subtract(position).unit())
	, m_viewWidth(viewWidth)
	, m_viewHeight(viewHeight)
	, m_right(StandardVectors::kUnitY.crossProduct(m_direction).unit().scale(m_viewWidth / 2))
	, m_down(m_right.crossProduct(m_direction).unit().scale(-m_viewHeight / 2).invert())
{

}

Color Camera::trace(const Scene& scene, double x, double y) const
{
	const Vector rayX = m_right.scale(x);
	const Vector rayY = m_down.scale(y);

	return Ray(m_position, m_direction.add(rayX).add(rayY)).trace(scene, 0);
}
