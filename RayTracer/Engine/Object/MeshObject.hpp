#pragma once

#include "Engine/Mesh.hpp"
#include "Engine/Object.hpp"

#include <memory>

class Material;

struct Vector;

class MeshObject final
	: public Object
{
public:
								MeshObject(const Transform& transform, std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh);
								~MeshObject() override = default;

// Object i/f:
protected:
	double						intersectWith(const Ray& ray) const override;
	void						getIntersectionProperties(const Ray& ray, const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const override;

private:
	Vector						normalAt(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vector& mix) const;
	Vector						uvAt(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vector& mix) const;

	double						intersectWith(const Ray& ray, const Vertex& v0, const Vertex& v1, const Vertex& v2) const;
	bool						pointOn(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex& v2) const;
	Vector						interpolate(const Vector& point, const Vertex& v0, const Vertex& v1, const Vertex& v2) const;

private:
	std::shared_ptr<Mesh>		m_mesh;
};
