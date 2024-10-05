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

struct Mesh
{
public:
	Vector						lowerCorner;
	Vector						upperCorner;

	std::vector<Vertex> 		vertices;
	std::vector<Triangle>		triangles;
};

std::shared_ptr<Mesh> BuildMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles);
