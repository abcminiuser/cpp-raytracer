#include "Engine/Object/MeshObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <stdexcept>

MeshObject::MeshObject(const Vector& position, const Vector& rotation, std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh)
	: Object(position, rotation, std::move(material))
	, m_mesh(std::move(mesh))
{
	if (! m_mesh)
		throw std::runtime_error("Mesh object create with no associated mesh.");
}

double MeshObject::intersectWith(const Ray& ray) const
{
	double distance = kNoIntersection;

	m_mesh->walk(
		[&](const Vector& lowerCorner, const Vector& upperCorner) -> bool
		{
			// Check if the ray intersects this node's bounding box.

			const auto t1 = lowerCorner.subtract(ray.position()).multiply(ray.directionInverse());
			const auto t2 = upperCorner.subtract(ray.position()).multiply(ray.directionInverse());

			const Vector minPoint = VectorUtils::MinPoint(t1, t2);
			const Vector maxPoint = VectorUtils::MaxPoint(t1, t2);

			const double tMin = VectorUtils::MaxComponent(minPoint);
			const double tMax = VectorUtils::MinComponent(maxPoint);

			if (tMax < 0)
			{
				// Intersection is behind us
				return false;
			}

			if (tMin > tMax)
			{
				// No intersection
				return false;
			}

			if (tMin >= distance)
			{
				// Intersected, but further away than our existing closest match
				return false;
			}

			// Intersected, search this node.
			return true;
		},
		[&](const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles)
		{
			// If we intersect, find the distance to the closest triangle in this node (if any).

			for (const auto& triangle : triangles)
			{
				const auto& [p0, p1, p2] = triangle;

				const auto& v0 = vertices[p0];
				const auto& v1 = vertices[p1];
				const auto& v2 = vertices[p2];

				distance = std::min(distance, intersectWith(ray, v0, v1, v2));
			}
		});

	return distance;
}

void MeshObject::getIntersectionProperties(const Vector& position, Vector& normal, Vector& uv) const
{
	bool found = false;

	m_mesh->walk(
		[&](const Vector& lowerCorner, const Vector& upperCorner) -> bool
		{
			// Check if this point is within this node's bounding box.

			if (found)
				return false;

			if (position.x() < lowerCorner.x() || position.x() > upperCorner.x())
				return false;

			if (position.y() < lowerCorner.y() || position.y() > upperCorner.y())
				return false;

			if (position.z() < lowerCorner.z() || position.z() > upperCorner.z())
				return false;

			// Our search point is contained in the bounding box, search this node.
			return true;
		},
		[&](const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles)
		{
			// If we intersect, find the distance to the closest triangle in this node (if any).

			for (const auto& triangle : triangles)
			{
				const auto& [p0, p1, p2] = triangle;

				const auto& v0 = vertices[p0];
				const auto& v1 = vertices[p1];
				const auto& v2 = vertices[p2];

				if (! pointOn(position, v0, v1, v2))
					continue;

				const Vector mix = interpolate(position, v0, v1, v2);

				normal	= normalAt(v0, v1, v2, mix);
				uv		= uvAt(v0, v1, v2, mix);

				found = true;
				return;
			}
		});

	if (! found)
	{
		normal	= StandardVectors::kUnitZ;
		uv		= Vector();
	}
}

Vector MeshObject::normalAt(const Vertex& v0, const Vertex& v1, const Vertex v2, const Vector& mix) const
{
	return v0.normal.scale(mix.x()).add(v1.normal.scale(mix.y())).add(v2.normal.scale(mix.z())).unit();
}

Vector MeshObject::uvAt(const Vertex& v0, const Vertex& v1, const Vertex v2, const Vector& mix) const
{
	return v0.texture.scale(mix.x()).add(v1.texture.scale(mix.y())).add(v2.texture.scale(mix.z()));
}

double MeshObject::intersectWith(const Ray& ray, const Vertex& v0, const Vertex& v1, const Vertex v2) const
{
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

	const Vector edge1 = v1.position.subtract(v0.position);
	const Vector edge2 = v2.position.subtract(v0.position);
	const Vector rayCrossE2 = ray.direction().crossProduct(edge2);

	const double det = edge1.dotProduct(rayCrossE2);
	if (std::abs(det) < kComparisonThreshold)
		return kNoIntersection;

	const double invDet = 1.0 / det;
	const Vector s = ray.position().subtract(v0.position);

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

bool MeshObject::pointOn(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex v2) const
{
	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html

	const Vector a = v0.position.subtract(point);
	const Vector b = v1.position.subtract(point);
	const Vector c = v2.position.subtract(point);

	const Vector u = b.crossProduct(c);
	const Vector v = c.crossProduct(a);
	const Vector w = a.crossProduct(b);

	if (u.dotProduct(v) < kComparisonThreshold)
		return false;

	if (u.dotProduct(w) < kComparisonThreshold)
		return false;

	return true;
}

Vector MeshObject::interpolate(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex v2) const
{
	// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates

	const Vector e0 = v1.position.subtract(v0.position);
	const Vector e1 = v2.position.subtract(v0.position);
	const Vector e2 = point.subtract(v0.position);

	const double d00 = e0.dotProduct(e0);
	const double d01 = e0.dotProduct(e1);
	const double d11 = e1.dotProduct(e1);
	const double d20 = e2.dotProduct(e0);
	const double d21 = e2.dotProduct(e1);
	const double denom = d00 * d11 - d01 * d01;

	const double v = (d11 * d20 - d01 * d21) / denom;
	const double w = (d00 * d21 - d01 * d20) / denom;
	const double u = 1.0 - v - w;

	return Vector(u, v, w);
}
