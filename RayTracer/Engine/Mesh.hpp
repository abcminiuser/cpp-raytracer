#pragma once

#include "Engine/BoundingBox.hpp"
#include "Engine/Vector.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <variant>
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

	const BoundingBox&		boundingBox() const
	{
		return m_root.boundingBox;
	}

	template <typename BBTestCallable, typename TriangleCallable>
	void					walk(BBTestCallable&& boundingBoxTest, TriangleCallable&& triangleTest) const
	{
		walk(boundingBoxTest, triangleTest, m_root);
	}

private:
	struct Node;

	using TriangleList	= std::vector<Triangle>;
	using ChildNodes	= std::array<std::unique_ptr<Node>, 8>;

	struct Node
	{
		BoundingBox								boundingBox;
		std::variant<TriangleList, ChildNodes>	contents;
	};

	template<class... Ts>
	struct VisitorOverloads : Ts... { using Ts::operator()...; };

	template<class... Ts>
	VisitorOverloads(Ts...) -> VisitorOverloads<Ts...>;

	template <typename BBTestCallable, typename TriangleCallable>
	void					walk(BBTestCallable&& boundingBoxTest, TriangleCallable&& triangleTest, const Node& node) const
	{
		if (! boundingBoxTest(node.boundingBox))
			return;

		std::visit(
			VisitorOverloads
			{
				[&](const TriangleList& triangles)
				{
					triangleTest(std::as_const(m_vertices), triangles);
				},
				[&](const ChildNodes& children)
				{
					for (const auto& c : children)
					{
						if (const auto* childNode = c.get())
							walk(boundingBoxTest, triangleTest, *childNode);
					}
				},
			},
			node.contents);
	}

	std::unique_ptr<Node>	partition(std::vector<Triangle> triangles, uint32_t depth);

	BoundingBox				boundingBoxForTriangles(const std::vector<Triangle>& triangles) const;

	std::vector<Triangle>	trianglesInBox(const BoundingBox& boundingBox, const std::vector<Triangle>& triangles) const;
	bool					boxContainsTriangle(const BoundingBox& boundingBox, const Triangle& triangle) const;

private:
	std::vector<Vertex> 	m_vertices;
	Node					m_root = {};
};
