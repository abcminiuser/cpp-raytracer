#include "Engine/Object/PlaneObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

PlaneObject::PlaneObject(std::shared_ptr<Material> material, const Vector& normal, double distance, double uvScaleFactor)
	: Object(normal.scale(distance), Vector(), std::move(material))
	, m_normal(normal)
	, m_uvScaleFactor(uvScaleFactor)
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
	const auto b = m_normal.dotProduct(ray.position());
	return -b / angle;
}

void PlaneObject::getIntersectionProperties(const Vector& position, Vector& normal, Vector& uv) const
{
	normal	= m_normal;
	uv		= uvAt(position, normal);
}

Vector PlaneObject::uvAt(const Vector& position, const Vector& normal) const
{
	const auto positionFromOrigin = position.scale(m_uvScaleFactor);

	auto u = positionFromOrigin.x();
	auto v = positionFromOrigin.z();

	u -= std::floor(u);
	v -= std::floor(v);

	return Vector(u, v, 0);
}
