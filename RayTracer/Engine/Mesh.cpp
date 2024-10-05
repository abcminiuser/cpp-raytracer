#include "Engine/Mesh.hpp"

std::shared_ptr<Mesh> BuildMesh(std::vector<Vertex> vertices, std::vector<Triangle> triangles)
{
	auto mesh = std::make_shared<Mesh>();

	mesh->vertices	= std::move(vertices);
	mesh->triangles = std::move(triangles);
	mesh->lowerCorner = StandardVectors::kMax;
	mesh->upperCorner = StandardVectors::kMin;

	for (const auto& t : mesh->triangles)
	{
		for (const auto& p : t)
		{
			mesh->lowerCorner = Vector::MinPoint(mesh->lowerCorner, mesh->vertices[p].position);
			mesh->upperCorner = Vector::MaxPoint(mesh->upperCorner, mesh->vertices[p].position);
		}
	}

	return mesh;
}
