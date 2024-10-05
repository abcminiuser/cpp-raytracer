#include "Engine/Object/PlaneObject.hpp"

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

PlaneObject::PlaneObject(const Vector& normal, double distance, std::shared_ptr<Texture> texture, double textureScaleFactor, const Material& material)
	: Object(normal.scale(distance), material)
	, m_texture(std::move(texture))
	, m_textureScaleFactor(textureScaleFactor)
	, m_normal(normal)
{

}

double PlaneObject::intersectWith(const Ray& ray) const
{
	const auto angle = ray.direction().dotProduct(m_normal);

	if (angle == 0)
	{
		// No intersection
		return kNoIntersection;
	}

	// Intersection at a single point (as we're infinitely thin)
	const auto b = m_normal.dotProduct(ray.position().subtract(m_position));
	return -b / angle;
}

Vector PlaneObject::normalAt(const Vector& position) const
{
	return m_normal;
}

Color PlaneObject::colorAt(const Scene& scene, const Ray& ray) const
{
	if (! m_texture)
		return Palette::kBlack;

	const auto rayFromOrigin = ray.position().subtract(m_position).scale(m_textureScaleFactor);

	auto u = rayFromOrigin.x();
	auto v = rayFromOrigin.z();

	return m_texture->colorAt(u, v);
}
