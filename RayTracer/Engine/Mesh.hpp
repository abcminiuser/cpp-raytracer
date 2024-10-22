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
	void					walk(const BBTestCallable& boundingBoxTest, const TriangleCallable& triangleTest) const
	{
		if (! m_root)
			return;

		walk(boundingBoxTest, triangleTest, m_root.get());
	}

private:
	struct Node
	{
		Vector									lowerCorner;
		Vector									upperCorner;

		std::vector<Triangle>					triangles;

		std::array<std::unique_ptr<Node>, 8>	children = {};
	};

	template <typename BBTestCallable, typename TriangleCallable>
	void					walk(const BBTestCallable& boundingBoxTest, const TriangleCallable& triangleTest, const Node* node) const
	{
		if (! boundingBoxTest(node->lowerCorner, node->upperCorner))
			return;

		if (node->triangles.empty())
		{
			for (const auto& c : node->children)
			{
				if (c)
					walk(boundingBoxTest, triangleTest, c.get());
			}

			return;
		}

		triangleTest(std::as_const(m_vertices), node->triangles);
	}

	std::unique_ptr<Node>	partition(std::vector<Triangle> triangles, uint32_t depth);

	std::vector<Triangle>	trianglesInBox(const Vector& lowerCorner, const Vector& upperCorner, const std::vector<Triangle>& triangles) const;
	bool					boxContainsTriangle(const Vector& lowerCorner, const Vector& upperCorner, const Triangle& triangle) const;

private:
	std::vector<Vertex> 			m_vertices;
	std::unique_ptr<Node>			m_root;
};
