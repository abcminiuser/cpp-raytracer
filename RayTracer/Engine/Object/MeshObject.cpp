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
	if (!m_mesh)
		throw std::runtime_error("Mesh object create with no associated mesh.");
}

double MeshObject::intersectWith(const Ray& ray) const
{
	double distance = kNoIntersection;

	// Cheaper to adjust the ray by our position here once and re-use it for the bounding box
	// and triangle intersection tests, than to offset it for every point.
	const auto rayAdjusted = Ray(ray.position().subtract(m_position), ray.direction());

	m_mesh->octree.walk(
		[&](const Vector& boundingBoxPosition, const Vector& boundingBoxSize, const std::vector<Triangle>& triangles) -> bool
		{
			const auto lowerCorner = Vector::MinPoint(boundingBoxPosition, boundingBoxPosition.add(boundingBoxSize));
			const auto upperCorner = Vector::MaxPoint(boundingBoxPosition, boundingBoxPosition.add(boundingBoxSize));

			const auto t1 = lowerCorner.subtract(rayAdjusted.position()).multiply(rayAdjusted.directionInverse());
			const auto t2 = upperCorner.subtract(rayAdjusted.position()).multiply(rayAdjusted.directionInverse());

			const Vector maxPoint = Vector::MaxPoint(t1, t2);
			const double tMax = std::min({ maxPoint.x(), maxPoint.y(), maxPoint.z() });

			if (tMax < 0)
			{
				// Intersection is behind us
				return false;
			}

			const Vector minPoint = Vector::MinPoint(t1, t2);
			const double tMin = std::max({ minPoint.x(), minPoint.y(), minPoint.z() });

			if (tMin >= tMax)
			{
				// No intersection
				return false;
			}

			for (const auto& t : triangles)
				distance = std::min(distance, intersectWith(rayAdjusted, t));

			return true;
		});

	return distance;
}

Vector MeshObject::normalAt(const Vector& position) const
{
	const Vector comparePos = position.subtract(m_position);

	Vector normal = StandardVectors::kUnitZ;

	m_mesh->octree.walk(
		[&](const Vector& boundingBoxPosition, const Vector& boundingBoxSize, const std::vector<Triangle>& triangles) -> bool
		{
			for (const auto& t : triangles)
			{
				if (! pointOn(comparePos, t))
					continue;

				const auto& [t0, t1, t2] = t;

				// Use the normal of the first vertex for now.
				normal = m_mesh->vertices[t0].normal;

				return false;
			}

			return true;
		});

	return normal;
}

Color MeshObject::colorAt(const Scene& scene, const Ray& ray) const
{
	if (! m_texture)
		return Palette::kBlack;

	const Vector comparePos = ray.position().subtract(m_position);

	Color color = Palette::kBlack;

	m_mesh->octree.walk(
		[&](const Vector& boundingBoxPosition, const Vector& boundingBoxSize, const std::vector<Triangle>& triangles) -> bool
		{
			for (const auto& t : triangles)
			{
				if (! pointOn(comparePos, t))
					continue;

				const auto& [t0, t1, t2] = t;

				// Use the texture coordinates of the first vertex for now.
				color = m_texture->colorAt(m_mesh->vertices[t0].texture.x(), m_mesh->vertices[t0].texture.y());

				return false;
			}

			return true;
		});

	return color;
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
	// https://blackpawn.com/texts/pointinpoly/

	const auto& [t0, t1, t2] = triangle;

	const auto& v0 = m_mesh->vertices[t0].position;
	const auto& v1 = m_mesh->vertices[t1].position;
	const auto& v2 = m_mesh->vertices[t2].position;

	constexpr auto SameSide =
		[](Vector p1, Vector p2, Vector a, Vector b)
		{
			Vector cp1 = b.subtract(a).crossProduct(p1.subtract(a));
			Vector cp2 = b.subtract(a).crossProduct(p2.subtract(a));
			return cp1.dotProduct(cp2) >= 0;
		};

	return SameSide(point, v0, v1, v2) && SameSide(point, v1, v0, v2) && SameSide(point, v2, v0, v1);
}
