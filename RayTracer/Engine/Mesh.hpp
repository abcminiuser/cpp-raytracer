#pragma once

#include "Engine/BoundingBox.hpp"
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

	BoundingBox				boundingBox() const
	{
		return m_root.boundingBox;
	}

	template <typename BBTestCallable, typename TriangleCallable>
	void					walk(BBTestCallable&& boundingBoxTest, TriangleCallable&& triangleTest) const
	{
		walk(boundingBoxTest, triangleTest, m_root);
	}

private:
	struct Node
	{
		BoundingBox								boundingBox;

		std::vector<Triangle>					triangles;

		std::array<std::unique_ptr<Node>, 8>	children = {};
	};

	template <typename BBTestCallable, typename TriangleCallable>
	void					walk(BBTestCallable&& boundingBoxTest, TriangleCallable&& triangleTest, const Node& node) const
	{
		if (! boundingBoxTest(node.boundingBox))
			return;

		if (node.triangles.empty())
		{
			for (const auto& c : node.children)
			{
				if (const auto* childNode = c.get())
					walk(boundingBoxTest, triangleTest, *childNode);
			}

			return;
		}
		else
		{
			triangleTest(std::as_const(m_vertices), node.triangles);
		}
	}

	std::unique_ptr<Node>	partition(std::vector<Triangle> triangles, uint32_t depth);

	BoundingBox				boundingBoxForTriangles(const std::vector<Triangle>& triangles) const;

	std::vector<Triangle>	trianglesInBox(const BoundingBox& boundingBox, const std::vector<Triangle>& triangles) const;
	bool					boxContainsTriangle(const BoundingBox& boundingBox, const Triangle& triangle) const;

private:
	std::vector<Vertex> 	m_vertices;
	Node					m_root = {};
};
