#include "Engine/Object/MeshObject.hpp"
#include "Engine/Object/BoxObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <stdexcept>

MeshObject::MeshObject(const Vector& position, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, const Material& material)
	: Object(position, material)
	, m_mesh(std::move(mesh))
	, m_texture(std::move(texture))
{
	if (! m_mesh)
		throw std::runtime_error("Mesh object create with no associated mesh.");
}

double MeshObject::intersectWith(const Ray& ray) const
{
	// Cheaper to adjust the ray by our position here once and re-use it for the bounding box
	// and triangle intersection tests, than to offset it for every point.
	const auto rayAdjusted = Ray(ray.position().subtract(m_position), ray.direction());

	const auto t1 = m_mesh->lowerCorner.subtract(rayAdjusted.position()).multiply(rayAdjusted.directionInverse());
	const auto t2 = m_mesh->upperCorner.subtract(rayAdjusted.position()).multiply(rayAdjusted.directionInverse());

	const Vector maxPoint = Vector::MaxPoint(t1, t2);
	const double tMax = std::min({ maxPoint.x(), maxPoint.y(), maxPoint.z() });

	if (tMax < 0)
	{
		// Intersection is behind us
		return kNoIntersection;
	}

	const Vector minPoint = Vector::MinPoint(t1, t2);
	const double tMin = std::max({ minPoint.x(), minPoint.y(), minPoint.z() });

	if (tMin >= tMax)
	{
		// No intersection
		return kNoIntersection;
	}

	double distance = kNoIntersection;
	for (const auto& triangle : m_mesh->triangles)
		distance = std::min(distance, intersectWith(rayAdjusted, triangle));

	return distance;
}

Vector MeshObject::normalAt(const Vector& position) const
{
	const Vector comparePos = position.subtract(m_position);

	for (const auto& triangle : m_mesh->triangles)
	{
		if (! pointOn(comparePos, triangle))
			continue;

		const auto& [p0, p1, p2] = triangle;

		const auto& n0 = m_mesh->vertices[p0].normal;
		const auto& n1 = m_mesh->vertices[p1].normal;
		const auto& n2 = m_mesh->vertices[p2].normal;

		const Vector mix = interpolate(comparePos, triangle).unit();

		const Vector normal = n0.scale(mix.x()).add(n1.scale(mix.y())).add(n2.scale(mix.z())).unit();
		return normal;
	}

	return StandardVectors::kUnitZ;
}

Color MeshObject::colorAt(const Scene& scene, const Vector& position, const Vector& normal) const
{
	if (! m_texture)
		return Palette::kBlack;

	const Vector comparePos = position.subtract(m_position);

	for (const auto& triangle : m_mesh->triangles)
	{
		if (! pointOn(comparePos, triangle))
			continue;

		const auto& [p0, p1, p2] = triangle;

		const auto& uv0 = m_mesh->vertices[p0].texture;
		const auto& uv1 = m_mesh->vertices[p1].texture;
		const auto& uv2 = m_mesh->vertices[p2].texture;

		const Vector mix = interpolate(comparePos, triangle).unit();

		const Vector uv = uv0.scale(mix.x()).add(uv1.scale(mix.y())).add(uv2.scale(mix.z())).unit();
		return m_texture->colorAt(uv.x(), uv.y());
	}

	return Palette::kBlack;
}

double MeshObject::intersectWith(const Ray& ray, const Triangle& triangle) const
{
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

	const auto& [t0, t1, t2] = triangle;

	const auto& v0 = m_mesh->vertices[t0].position;
	const auto& v1 = m_mesh->vertices[t1].position;
	const auto& v2 = m_mesh->vertices[t2].position;

	const Vector edge1 = v1.subtract(v0);
	const Vector edge2 = v2.subtract(v0);
	const Vector rayCrossE2 = ray.direction().crossProduct(edge2);

	const double det = edge1.dotProduct(rayCrossE2);
	if (std::abs(det) < kComparisonThreshold)
		return kNoIntersection;

	const double invDet = 1.0 / det;
	const Vector s = ray.position().subtract(v0);

	const double u = invDet * s.dotProduct(rayCrossE2);
	if (u < 0 || u > 1)
		return kNoIntersection;

	const Vector sCrossE1 = s.crossProduct(edge1);

	const double v = invDet * ray.direction().dotProduct(sCrossE1);
	if (v < 0 || u + v > 1)
		return kNoIntersection;

	const double t = invDet * edge2.dotProduct(sCrossE1);
	if (t < kComparisonThreshold)
		return kNoIntersection;

	return t;
}

bool MeshObject::pointOn(const Vector& point, const Triangle& triangle) const
{
	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html

	const auto& [p0, p1, p2] = triangle;

	const auto& v0 = m_mesh->vertices[p0].position;
	const auto& v1 = m_mesh->vertices[p1].position;
	const auto& v2 = m_mesh->vertices[p2].position;

	const Vector a = v0.subtract(point);
	const Vector b = v1.subtract(point);
	const Vector c = v2.subtract(point);

	const Vector u = b.crossProduct(c);
	const Vector v = c.crossProduct(a);
	const Vector w = a.crossProduct(b);

	if (u.dotProduct(v) < kComparisonThreshold)
		return false;

	if (u.dotProduct(w) < kComparisonThreshold)
		return false;

	return true;
}

Vector MeshObject::interpolate(const Vector& point, const Triangle& triangle) const
{
	// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates

	const auto& [p0, p1, p2] = triangle;

	const auto& v0 = m_mesh->vertices[p0].position;
	const auto& v1 = m_mesh->vertices[p1].position;
	const auto& v2 = m_mesh->vertices[p2].position;

	const Vector e0 = v1.subtract(v0);
	const Vector e1 = v2.subtract(v0);
	const Vector e2 = point.subtract(v0);

	double d00 = e0.dotProduct(e0);
	double d01 = e0.dotProduct(e1);
	double d11 = e1.dotProduct(e1);
	double d20 = e2.dotProduct(e0);
	double d21 = e2.dotProduct(e1);
	double denom = d00 * d11 - d01 * d01;

	double v = (d11 * d20 - d01 * d21) / denom;
	double w = (d00 * d21 - d01 * d20) / denom;
	double u = 1.0f - v - w;

	return Vector(u, v, w);
}
