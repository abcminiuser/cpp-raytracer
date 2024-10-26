#include "Engine/Mesh.hpp"

namespace
{
	constexpr auto kMaxOctreePartitionDepth		= 6;
	constexpr auto kMinTrianglesForPartition	= 25;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
	: m_vertices(std::move(vertices))
{
	if (triangles.empty())
		return;

	m_vertices.shrink_to_fit();

	// First find the bounding box for all triangles in the mesh.
	Vector minPoint = StandardVectors::kMax;
	Vector maxPoint = StandardVectors::kMin;
	for (const auto& t : triangles)
	{
		for (const auto& p : t)
		{
			minPoint = VectorUtils::MinPoint(minPoint, m_vertices[p].position);
			maxPoint = VectorUtils::MaxPoint(maxPoint, m_vertices[p].position);
		}
	}

	const Vector position	= minPoint;
	const Vector size		= maxPoint - minPoint;

	printf("Partitioning mesh %s size %s - %zu triangles\n", position.string().c_str(), size.string().c_str(), triangles.size());

	// Now build a tree of all the triangles, storing a bounding box for
	// each node, along with a list of triangles inside that bounding box
	// and a pair of sub-nodes for more triangles that are inside the left
	// or right side of the bounding box.
	m_root = partition(std::move(triangles), 0);

	size_t nodeCount = 0;
	walk(
		[&](const BoundingBox&) -> bool
		{
			nodeCount++;
			return true;
		},
		[](const std::vector<Vertex>&, const std::vector<Triangle>&)
		{
			// NOP
		});
	printf("Partitioning complete, %zu nodes.\n", nodeCount);
}

std::unique_ptr<Mesh::Node> Mesh::partition(std::vector<Triangle> triangles, uint32_t depth)
{
	if (triangles.empty())
		return nullptr;

	auto node = std::make_unique<Node>();
	node->boundingBox	= BoundingBox{ .lower = StandardVectors::kMax, .upper = StandardVectors::kMin };
	node->triangles		= std::move(triangles);

	for (const auto& t : node->triangles)
	{
		for (const auto& p : t)
		{
			node->boundingBox.lower = VectorUtils::MinPoint(node->boundingBox.lower, m_vertices[p].position);
			node->boundingBox.upper = VectorUtils::MaxPoint(node->boundingBox.upper, m_vertices[p].position);
		}
	}

	// If we've hit our depth limit, we'll just adopt all the matching triangles here and bail out.
	if (depth >= kMaxOctreePartitionDepth)
		return node;

	// If we have fewer triangles than the set limit, we don't partition further.
	if (node->triangles.size() < kMinTrianglesForPartition)
		return node;

	// If we matched too many triangles for this node, split it up into eight smaller cubes within our bounding box.
	const auto partitionSize = (node->boundingBox.upper - node->boundingBox.lower) * 0.5;

	const auto oX = partitionSize.x();
	const auto oY = partitionSize.y();
	const auto oZ = partitionSize.z();

	const std::array kOffsets =
		{
			Vector(0, 0, 0),
			Vector(oX, 0, 0),
			Vector(0, oY, 0),
			Vector(0, 0, oZ),
			Vector(oX, oY, 0),
			Vector(oX, 0, oZ),
			Vector(0, oY, oZ),
			Vector(oX, oY, oZ)
		};

	std::array<std::vector<Triangle>, 8> childrenTriangles;

	// Determine which of our triangles intersect each child's bounding box.
	for (size_t i = 0; i < 8; i++)
		childrenTriangles[i] = trianglesInBox(BoundingBox{ .lower = node->boundingBox.lower + kOffsets[i], .upper = node->boundingBox.lower + kOffsets[i] + partitionSize }, node->triangles);

	// Now we've partitioned our triangles, remove them from our node.
	node->triangles.clear();

	// Build child nodes from the list of triangles in each child's bounding box.
	for (size_t i = 0; i < 8; i++)
		node->children[i] = partition(std::move(childrenTriangles[i]), depth + 1);

	return node;
}

std::vector<Triangle> Mesh::trianglesInBox(const BoundingBox& boundingBox, const std::vector<Triangle>& triangles) const
{
	std::vector<Triangle> matchedTriangles;

	matchedTriangles.reserve(triangles.size());
	for (const auto& triangle : triangles)
	{
		if (boxContainsTriangle(boundingBox, triangle))
		    matchedTriangles.push_back(triangle);
	}
	matchedTriangles.shrink_to_fit();

	return matchedTriangles;
}

bool Mesh::boxContainsTriangle(const BoundingBox& boundingBox, const Triangle& triangle) const
{
	// Determine the bounding box for this triangle.
	BoundingBox triangleBoundingBox { .lower = StandardVectors::kMax, .upper = StandardVectors::kMin };
	for (const auto& p : triangle)
	{
		triangleBoundingBox.lower = VectorUtils::MinPoint(triangleBoundingBox.lower, m_vertices[p].position);
		triangleBoundingBox.upper = VectorUtils::MaxPoint(triangleBoundingBox.upper, m_vertices[p].position);
	}

	return boundingBox.intersects(triangleBoundingBox);
}
