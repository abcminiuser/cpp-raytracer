#include "Camera.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

Camera::Camera()
	: Camera(StandardVectors::kOrigin, Vector(0, 0, 1), 4, 4)
{}

Camera::Camera(Vector pos, Vector dir, float lensWidth, float lensHeight)
	: m_position(std::move(pos))
	, m_direction(dir.normalized())
	, m_lensWidth(lensWidth)
	, m_lensHeight(lensHeight)
	, m_right(StandardVectors::kUnitY.crossProduct(m_direction).normalized().scale(m_lensWidth / 2))
	, m_up(m_right.crossProduct(m_direction).normalized().scale(-m_lensHeight / 2))
{}

Color Camera::trace(const Scene& scene, size_t screenX, size_t screenY) const
{
	const Vector rayX = m_right.scale(static_cast<float>(screenX));
	const Vector rayY = m_up.scale(-static_cast<float>(screenY));

	return Ray(m_position, m_direction.add(rayX).add(rayY)).trace(scene);
}
