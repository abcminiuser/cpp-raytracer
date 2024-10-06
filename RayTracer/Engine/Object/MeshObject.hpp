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
	Vector						normalAt(const Triangle& triangle, const Vector& mix) const;
	Color						colorAt(const Triangle& triangle, const Vector& mix) const;

	double						intersectWith(const Ray& ray, const Triangle& triangle) const;
	bool						pointOn(const Vector& point, const Triangle& triangle) const;
	Vector						interpolate(const Vector& point, const Triangle& triangle) const;

private:
	std::shared_ptr<Mesh>		m_mesh;
	std::shared_ptr<Texture>	m_texture;
};
