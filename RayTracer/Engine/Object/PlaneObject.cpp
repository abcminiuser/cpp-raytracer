#include "PlaneObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <cmath>
#include <numbers>

PlaneObject::PlaneObject(const Vector& normal, double distance, const Material& material, double textureScaleFactor)
	: Object(normal.scale(distance), material)
	, m_normal(normal)
	, m_textureScaleFactor(textureScaleFactor)
{

}

Object::IntersectionDistances PlaneObject::intersectWith(const Ray& ray) const
{
	const auto angle = ray.direction().dotProduct(m_normal);

	if (angle == 0)
	{
		// No intersection
		return { kNoIntersection, kNoIntersection };
	}
	else
	{
		// Intersection at a single point (as we're infinitely thin)
		const auto b = m_normal.dotProduct(ray.position().subtract(m_position));
        return { -b / angle, kNoIntersection };
	}
}

Vector PlaneObject::normalAt(const Vector& position) const
{
	return m_normal;
}

Color PlaneObject::colorAt(const Scene& scene, const Ray& ray) const
{
	if (! m_material.texture)
		return Palette::kBlack;

	const auto rayFromOrigin = ray.position().subtract(m_position).scale(m_textureScaleFactor);

	auto u = rayFromOrigin.x();
	auto v = rayFromOrigin.z();
	u -= std::floor(u);
	v -= std::floor(v);

	return m_material.texture->colorAt(u, v);
}
