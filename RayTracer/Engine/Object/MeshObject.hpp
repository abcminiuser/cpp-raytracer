#pragma once

#include "Engine/Material.hpp"
#include "Engine/Mesh.hpp"
#include "Engine/Object.hpp"

#include <array>
#include <memory>
#include <vector>

class Texture;

struct Vector;

class MeshObject
	: public Object
{
public:
								MeshObject(const Vector& position, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, const Material& material);
								~MeshObject() override = default;

// Object i/f:
protected:
	double						intersectWith(const Ray& ray) const override;
	void						getIntersectionProperties(const Vector& position, Vector& normal, Color& color) const override;

private:
	Vector						normalAt(const Vertex& v0, const Vertex& v1, const Vertex v2, const Vector& mix) const;
	Color						colorAt(const Vertex& v0, const Vertex& v1, const Vertex v2, const Vector& mix) const;

	double						intersectWith(const Ray& ray, const Vertex& v0, const Vertex& v1, const Vertex v2) const;
	bool						pointOn(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex v2) const;
	Vector						interpolate(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex v2) const;

private:
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Texture>	m_texture;
};
