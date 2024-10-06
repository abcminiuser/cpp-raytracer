#include "Engine/Mesh.hpp"

std::shared_ptr<Mesh> BuildMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
{
	auto mesh = std::make_shared<Mesh>();

	mesh->vertices	= std::move(vertices);
	mesh->triangles = std::move(triangles);
	mesh->lowerCorner = StandardVectors::kMax;
	mesh->upperCorner = StandardVectors::kMin;

	for (const auto& triangle : mesh->triangles)
	{
		for (const auto& point : triangle)
		{
			const auto& vertex = mesh->vertices[point];

			mesh->lowerCorner = Vector::MinPoint(mesh->lowerCorner, vertex.position);
			mesh->upperCorner = Vector::MaxPoint(mesh->upperCorner, vertex.position);
		}
	}

	return mesh;
}
