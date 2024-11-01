#include "Engine/Object/MeshObject.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <stdexcept>

MeshObject::MeshObject(const Transform& transform, std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh)
	: Object(transform, std::move(material))
	, m_mesh(std::move(mesh))
{
	if (! m_mesh)
		throw std::runtime_error("Mesh object created with no associated mesh.");
}

double MeshObject::intersectWith(const Ray& ray) const
{
	double distance = Ray::kNoIntersection;

	m_mesh->walk(
		[&](const BoundingBox& boundingBox) -> bool
		{
			return boundingBox.intersect(ray) < distance;
		},
		[&](const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles)
		{
			// If we intersect, find the distance to the closest triangle in this node (if any).

			for (const auto& triangle : triangles)
			{
				const auto& [p0, p1, p2] = triangle;

				const Vertex& v0 = vertices[p0];
				const Vertex& v1 = vertices[p1];
				const Vertex& v2 = vertices[p2];

				distance = std::min(distance, intersectWith(ray, v0, v1, v2));
			}
		});

	return distance;
}

void MeshObject::getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const
{
	bool found = false;

	m_mesh->walk(
		[&](const BoundingBox& boundingBox) -> bool
		{
			// If we already found a matching point, don't search this node.
			if (found)
				return false;

			// If out search point is contained in the bounding box, search this node.
			return boundingBox.contains(position);
		},
		[&](const std::vector<Vertex>& vertices, const std::vector<Triangle>& triangles)
		{
			// If we intersect, find the distance to the closest triangle in this node (if any).

			for (const auto& triangle : triangles)
			{
				const auto& [p0, p1, p2] = triangle;

				const Vertex& v0 = vertices[p0];
				const Vertex& v1 = vertices[p1];
				const Vertex& v2 = vertices[p2];

				if (! pointOn(position, v0, v1, v2))
					continue;

				const Vector mix = interpolate(position, v0, v1, v2);

				normal		= normalAt(v0, v1, v2, mix);
				tangent		= (v1.position - v0.position).unit();
				bitangent	= (v2.position - v1.position).unit();
				uv			= uvAt(v0, v1, v2, mix);

				found = true;
				return;
			}
		});

	if (! found)
	{
		normal		= StandardVectors::kUnitZ;
		tangent		= StandardVectors::kUnitY;
		bitangent	= StandardVectors::kUnitX;
		uv			= StandardVectors::kZero;
	}
}

Vector MeshObject::normalAt(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vector& mix) const
{
	return (v0.normal * mix.x() + v1.normal * mix.y() + v2.normal * mix.z()).unit();
}

Vector MeshObject::uvAt(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vector& mix) const
{
	return (v0.texture * mix.x() + v1.texture * mix.y() + v2.texture * mix.z());
}

double MeshObject::intersectWith(const Ray& ray, const Vertex& v0, const Vertex& v1, const Vertex& v2) const
{
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

	const Vector edge1 = v1.position - v0.position;
	const Vector edge2 = v2.position - v0.position;

	const Vector rayCrossE2 = ray.direction().crossProduct(edge2);

	const double det = edge1.dotProduct(rayCrossE2);
	if (std::abs(det) < kComparisonThreshold)
		return Ray::kNoIntersection;

	const double invDet = 1.0 / det;
	const Vector s = ray.position() - v0.position;

	const double u = invDet * s.dotProduct(rayCrossE2);
	if (u < 0 || u > 1)
		return Ray::kNoIntersection;

	const Vector sCrossE1 = s.crossProduct(edge1);

	const double v = invDet * ray.direction().dotProduct(sCrossE1);
	if (v < 0 || u + v > 1)
		return Ray::kNoIntersection;

	const double t = invDet * edge2.dotProduct(sCrossE1);
	if (t < kComparisonThreshold)
		return Ray::kNoIntersection;

	return t;
}

bool MeshObject::pointOn(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex& v2) const
{
	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter4/point_in_triangle.html

	const Vector a = v0.position - point;
	const Vector b = v1.position - point;
	const Vector c = v2.position - point;

	const Vector u = b.crossProduct(c);
	const Vector v = c.crossProduct(a);
	const Vector w = a.crossProduct(b);

	if (u.dotProduct(v) < kComparisonThreshold)
		return false;

	if (u.dotProduct(w) < kComparisonThreshold)
		return false;

	return true;
}

Vector MeshObject::interpolate(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex& v2) const
{
	// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates

	const Vector e0 = v1.position - v0.position;
	const Vector e1 = v2.position - v0.position;
	const Vector e2 = point - v0.position;

	const double d00 = e0.dotProduct(e0);
	const double d01 = e0.dotProduct(e1);
	const double d11 = e1.dotProduct(e1);
	const double d20 = e2.dotProduct(e0);
	const double d21 = e2.dotProduct(e1);
	const double invDenom = 1.0 / (d00 * d11 - d01 * d01);

	const double v = (d11 * d20 - d01 * d21) * invDenom;
	const double w = (d00 * d21 - d01 * d20) * invDenom;
	const double u = 1.0 - v - w;

	return Vector(u, v, w);
}
