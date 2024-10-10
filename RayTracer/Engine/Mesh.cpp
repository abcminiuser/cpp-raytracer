#include "Engine/Mesh.hpp"

namespace
{
	constexpr auto kMaxOctreePartitionDepth = 4;
	constexpr auto kMinTrianglesForOctreePartition = 1;

	bool BoxContainsTriangle(const Vector& lowerCorner, const Vector& upperCorner, const std::vector<Vertex>& vertices, const Triangle& triangle)
	{
		// Determine the bounding box for this triangle.
		Vector triangleLowerCorner = StandardVectors::kMax;
		Vector triangleUpperCorner = StandardVectors::kMin;
		for (const auto& p : triangle)
		{
			triangleLowerCorner = Vector::MinPoint(triangleLowerCorner, vertices[p].position);
			triangleUpperCorner = Vector::MaxPoint(triangleUpperCorner, vertices[p].position);
		}

		const auto xInBounds = lowerCorner.x() <= triangleUpperCorner.x() && triangleLowerCorner.x() <= upperCorner.x();
		const auto yInBounds = lowerCorner.y() <= triangleUpperCorner.y() && triangleLowerCorner.y() <= upperCorner.y();
		const auto zInBounds = lowerCorner.z() <= triangleUpperCorner.z() && triangleLowerCorner.z() <= upperCorner.z();

		return xInBounds && yInBounds && zInBounds;
	}
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
	: m_vertices(std::move(vertices))
{
	if (triangles.empty())
		return;

	// First find the bounding box for all triangles in the mesh.
	Vector minPoint = StandardVectors::kMax;
	Vector maxPoint = StandardVectors::kMin;
	for (const auto& t : triangles)
	{
		for (const auto& p : t)
		{
			minPoint = Vector::MinPoint(minPoint, m_vertices[p].position);
			maxPoint = Vector::MaxPoint(maxPoint, m_vertices[p].position);
		}
	}

	// Now build a tree of all the triangles, storing a bounding box for
	// each node, along with a list of triangles inside that bounding box
	// and a pair of sub-nodes for more triangles that are inside the left
	// or right side of the bounding box.
	const auto position	= minPoint;
	const auto size		= maxPoint.subtract(minPoint);
	printf("Partitioning mesh (%f %f %f) size (%f %f %f) - %zu triangles\n", position.x(), position.y(), position.z(), size.x(), size.y(), size.z(), triangles.size());

	partition(position, size, m_vertices, 0, std::move(triangles));

	printf("Octree built, %zu nodes.\n", m_elements.size());
}

size_t Mesh::partition(const Vector& position, const Vector& size, const std::vector<Vertex>& vertices, uint32_t depth, std::vector<Triangle> triangles)
{
	const size_t nodeIndex = m_elements.size();
	auto& node = m_elements.emplace_back(Node());

	node.lowerCorner	= position;
	node.upperCorner	= position.add(size);

	// Find all triangles that are within this node's bounding box.
	for (const auto& t : triangles)
	{
		if (BoxContainsTriangle(node.lowerCorner, node.upperCorner, vertices, t))
		    node.triangles.push_back(t);
	}

	// If this node matched no triangles, just return a null node to save space now and processing time later.
	if (node.triangles.empty())
	{
		m_elements.pop_back();
		return 0;
	}

	// If we've hit our depth limit, we'll just adopt all the matching triangles here and bail out.
	if (depth >= kMaxOctreePartitionDepth)
		return nodeIndex;

	// If we matched too many triangles for this node, split it up into eight smaller cubes within our bounding box.
	if (node.triangles.size() > kMinTrianglesForOctreePartition)
	{
		const auto partitionSize = size.scale(0.5);

		const auto oX = partitionSize.x();
		const auto oY = partitionSize.y();
		const auto oZ = partitionSize.z();

		std::swap(triangles, node.triangles);
		node.triangles.clear();

		std::array<size_t, 8> childrenIndexes
			{
				partition(position.add(Vector(0, 0, 0)), partitionSize, vertices, depth + 1, triangles),
				partition(position.add(Vector(oX, 0, 0)), partitionSize, vertices, depth + 1, triangles),
				partition(position.add(Vector(0, oY, 0)), partitionSize, vertices, depth + 1, triangles),
				partition(position.add(Vector(oX, oY, 0)), partitionSize, vertices, depth + 1, triangles),

				partition(position.add(Vector(0, 0, oZ)), partitionSize, vertices, depth + 1, triangles),
				partition(position.add(Vector(oX, 0, oZ)), partitionSize, vertices, depth + 1, triangles),
				partition(position.add(Vector(0, oY, oZ)), partitionSize, vertices, depth + 1, triangles),
				partition(position.add(Vector(oX, oY, oZ)), partitionSize, vertices, depth + 1, triangles)
			};

		// Check if we've ended up with all our divided triangles in a single, smaller octant.
		size_t orphanChild = 0;
		for (const auto& c : childrenIndexes)
		{
			if (! c)
				continue;

			if (orphanChild)
			{
				orphanChild = 0;
				break;
			}

			orphanChild = c;
		}

		// If all the triangles are in one child, just adopt its bounding box and contents and remove it,
		// to avoid redundant bounding box checks later on.
		if (orphanChild)
		{
			m_elements[nodeIndex] = m_elements[orphanChild];
			m_elements.pop_back();
		}
		else
		{
			m_elements[nodeIndex].childrenIndexes = childrenIndexes;
		}
	}

	return nodeIndex;
}
