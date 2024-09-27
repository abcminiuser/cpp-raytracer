#include "Ray.hpp"

Ray::Ray(Vector pos, Vector dir)
	: m_position(std::move(pos))
	, m_direction(dir.normalized())
{}

Color Ray::trace(const Scene& scene) const
{
	return scene.background;
}
