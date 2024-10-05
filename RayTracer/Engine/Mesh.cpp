#include "Engine/Mesh.hpp"

namespace
{
	constexpr auto kMaxOctreePartitionDepth = 16;
	constexpr auto kMinTrianglesForOctreePartition = 64;

	bool BoxContainsTriangle(const Vector& position, const Vector& size, const std::vector<Vertex>& vertices, const Triangle& triangle)
	{
		for (const auto& p : triangle)
		{
			const auto dist = vertices[p].position.subtract(position);

			if ((dist.x() < 0) || dist.x() > size.x())
				continue;
			else if ((dist.y() < 0) || dist.y() > size.y())
				continue;
			else if ((dist.z() < 0) || dist.z() > size.z())
				continue;

			return true;
		}

		return false;
	}
}

Octree::Octree(const std::vector<Vertex>& vertices, std::vector<Triangle> triangles)
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
			minPoint = Vector::MinPoint(minPoint, vertices[p].position);
			maxPoint = Vector::MaxPoint(maxPoint, vertices[p].position);
		}
	}

	// Now build a tree of all the triangles, storing a bounding box for
	// each node, along with a list of triangles inside that bounding box
	// and a pair of sub-nodes for more triangles that are inside the left
	// or right side of the bounding box.
	const auto position	= minPoint;
	const auto size		= maxPoint.subtract(minPoint);
	printf("Partitioning mesh (%f %f %f) size (%f %f %f) - %zu triangles\n", position.x(), position.y(), position.z(), size.x(), size.y(), size.z(), triangles.size());

	partition(position, size, vertices, 0, std::move(triangles));

	printf("Octree built, %zu nodes.\n", m_elements.size());
}

size_t Octree::partition(const Vector& position, const Vector& size, const std::vector<Vertex>& vertices, uint32_t depth, std::vector<Triangle> triangles)
{
	Node node {};
	node.boundingBoxPosition	= position;
	node.boundingBoxSize		= size;

	if (depth >= kMaxOctreePartitionDepth)
	{
		// Max subdivision limit hit, retain all remaining triangles in this node.
		node.triangles = std::move(triangles);

		printf("Partitioned (%f %f %f) size (%f %f %f) - %zu triangles (depth limit)\n", position.x(), position.y(), position.z(), size.x(), size.y(), size.z(), node.triangles.size());

		const auto insertPos = m_elements.size();
		m_elements.push_back(std::move(node));
		return insertPos;
	}

	// Find all triangles that are within this node's bounding box.
	for (const auto& t : triangles)
	{
		if (BoxContainsTriangle(position, size, vertices, t))
		    node.triangles.push_back(t);
	}

	// If this node matched no triangles, just return a null node to save space now and processing time later.
	if (node.triangles.empty())
	{
		printf("Ignored (%f %f %f) size (%f %f %f) - no triangles\n", position.x(), position.y(), position.z(), size.x(), size.y(), size.z());
		return 0;
	}

	// If we matched too many triangles for this node, split it up into eight smaller cubes within our bounding box.
	if (node.triangles.size() >= kMinTrianglesForOctreePartition)
	{
		const auto partitionSize = size.scale(1);

		const auto oX = partitionSize.x();
		const auto oY = partitionSize.y();
		const auto oZ = partitionSize.z();

		printf("Subdividing (%f %f %f) size (%f %f %f) - %zu triangles\n", position.x(), position.y(), position.z(), size.x(), size.y(), size.z(), node.triangles.size());

		node.childrenIndexes =
			{
				partition(position.add(Vector(0,  0,  0)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(0, oY,  0)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(0,  0, oZ)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(0, oZ, oZ)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(oX,  0,  0)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(oX, oY,  0)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(oX,  0, oZ)), partitionSize, vertices, depth + 1, node.triangles),
				partition(position.add(Vector(oX, oY, oZ)), partitionSize, vertices, depth + 1, node.triangles),
			};

		node.triangles.clear();
	}
	else
	{
		printf("Partitioned (%f %f %f) size (%f %f %f) - %zu triangles\n", position.x(), position.y(), position.z(), size.x(), size.y(), size.z(), node.triangles.size());
	}

	const auto insertPos = m_elements.size();
	m_elements.push_back(std::move(node));
	return insertPos;
}

std::shared_ptr<Mesh> BuildMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
{
	auto mesh = std::make_shared<Mesh>();

	mesh->vertices	= std::move(vertices);
	mesh->octree	= Octree(mesh->vertices, std::move(triangles));

	return mesh;
}
