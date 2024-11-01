#include "Engine/Object/PlaneObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

PlaneObject::PlaneObject(std::shared_ptr<Material> material, const Vector& normal, double distance, double uvScaleFactor)
	: Object(Transform(normal * distance, StandardVectors::kZero, StandardVectors::kUnit), std::move(material))
	, m_normal(normal)
	, m_uvScaleFactor(uvScaleFactor)
{
	m_tangent = StandardVectors::kUnitX.crossProduct(normal).unit();
	if (m_tangent.length() == 0)
		m_tangent = StandardVectors::kUnitZ.crossProduct(normal).unit();

	m_bitangent = m_normal.crossProduct(m_tangent);
}

double PlaneObject::intersectWith(const Ray& ray) const
{
	const auto angle = ray.direction().dotProduct(m_normal);

	if (angle == 0)
	{
		// No intersection
		return Ray::kNoIntersection;
	}

	// Intersection at a single point (as we're infinitely thin)
	const auto b = m_normal.dotProduct(ray.position());
	return -b / angle;
}

void PlaneObject::getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
{
	normal		= m_normal;
	tangent		= m_tangent;
	bitangent	= m_bitangent;
	uv			= uvAt(position, normal);
}

Vector PlaneObject::uvAt(const Vector& position, const Vector& normal) const
{
	const auto positionFromOrigin = position * m_uvScaleFactor;

	auto u = m_tangent.dotProduct(positionFromOrigin);
	auto v = m_bitangent.dotProduct(positionFromOrigin);

	return Vector(u, v, 0);
}
