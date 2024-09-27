#pragma once

#include "Material.hpp"
#include "Object.hpp"
#include "Vector.hpp"

class Sphere
	: public Object
{
public:
							Sphere(Vector position, float radius, Material material);
							~Sphere() override = default;

// Object i/f:
protected:
	IntersectionDistances	intersectWith(const Ray& ray) const override;
	Vector					normalAt(const Vector& position) const override;
	Color					colorAt(const Scene& scene, const Ray& ray) const override;

private:
	Vector					m_position;
	float					m_radius;
	Material				m_material;
};
