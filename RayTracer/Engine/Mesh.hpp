#pragma once

#include "Engine/Vector.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

using Triangle = std::array<size_t, 3>;

struct Vertex
{
	Vector	position;
	Vector	normal;
	Vector	texture;
};

class Mesh
{
public:
			Mesh() = default;

			Mesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles);

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
		Vector						lowerCorner;
		Vector						upperCorner;

		std::vector<Triangle>		triangles;

		std::array<size_t, 8>		childrenIndexes;
	};

	template <typename Callable>
	void	walk(const Callable& callable, const Node& node)
	{
		if (! callable(node.lowerCorner, node.upperCorner, std::as_const(m_vertices), node.triangles))
			return;

		for (const auto& c : node.childrenIndexes)
		{
			if (c)
				walk(callable, m_elements[c]);
		}
	}

	size_t	partition(const Vector& position, const Vector& size, const std::vector<Vertex>& vertices, uint32_t depth, std::vector<Triangle> triangles);

private:
	std::vector<Vertex> 			m_vertices;
	std::vector<Node>				m_elements;
};
