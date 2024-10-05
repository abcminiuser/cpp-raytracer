#pragma once

#include "Engine/Vector.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

using Triangle = std::array<uint32_t, 3>;

struct Vertex
{
	Vector	position;
	Vector	normal;
	Vector	texture;
};

class Octree
{
public:
	Octree() = default;

	Octree(const std::vector<Vertex>& vertices, std::vector<Triangle> triangles);

	template <typename Callable>
	void	walk(const Callable& callable)
	{
		if (m_elements.empty())
			return;

		walk(callable, m_elements[0]);
	}

private:
	struct Node
	{
		Vector						boundingBoxPosition;
		Vector						boundingBoxSize;

		std::vector<Triangle>		triangles;

		std::array<size_t, 8>		childrenIndexes;
	};

	template <typename Callable>
	void	walk(const Callable& callable, const Node& node)
	{
		if (! callable(node.boundingBoxPosition, node.boundingBoxSize, node.triangles))
			return;

		for (const auto& c : node.childrenIndexes)
		{
			if (! c)
				continue;

			walk(callable, m_elements[c]);
		}
	}

	size_t	partition(const Vector& position, const Vector& size, const std::vector<Vertex>& vertices, uint32_t depth, std::vector<Triangle> triangles);

private:
	std::vector<Node>				m_elements;
};

struct Mesh
{
public:
	std::vector<Vertex> 			vertices;
	Octree							octree;
};

std::shared_ptr<Mesh> BuildMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles);
