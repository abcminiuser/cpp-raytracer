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

	template <typename BBTestCallable, typename TriangleCallable>
	void	walk(const BBTestCallable& boundingBoxTest, const TriangleCallable& triangleTest)
	{
		if (m_elements.empty())
			return;

		walk(boundingBoxTest, triangleTest, m_elements[0]);
	}

private:
	struct Node
	{
		Vector						lowerCorner;
		Vector						upperCorner;

		std::vector<Triangle>		triangles;

		std::array<size_t, 8>		childrenIndexes = {};
	};

	template <typename BBTestCallable, typename TriangleCallable>
	void	walk(const BBTestCallable& boundingBoxTest, const TriangleCallable& triangleTest, const Node& node)
	{
		if (! boundingBoxTest(node.lowerCorner, node.upperCorner))
			return;

		if (node.triangles.empty())
		{
			for (const auto& c : node.childrenIndexes)
			{
				if (c)
					walk(boundingBoxTest, triangleTest, m_elements[c]);
			}

			return;
		}

		triangleTest(std::as_const(m_vertices), node.triangles);
	}

	size_t	partition(const Vector& position, const Vector& size, uint32_t depth, std::vector<Triangle> triangles);
	bool	boxContainsTriangle(const Vector& lowerCorner, const Vector& upperCorner, const Triangle& triangle);

private:
	std::vector<Vertex> 			m_vertices;
	std::vector<Node>				m_elements;
};
