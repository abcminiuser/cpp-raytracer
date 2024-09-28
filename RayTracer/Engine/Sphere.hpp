#pragma once

#include "Material.hpp"
#include "Object.hpp"

struct Vector;

class Sphere
	: public Object
{
public:
							Sphere(Vector position, Material material, float radius);
							~Sphere() override = default;

// Object i/f:
protected:
	IntersectionDistances	intersectWith(const Ray& ray) const override;
	Vector					normalAt(const Vector& position) const override;
	Color					colorAt(const Scene& scene, const Ray& ray) const override;

private:
	float					m_radius;
};
