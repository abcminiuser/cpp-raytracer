#pragma once

#include "Engine/Material.hpp"
#include "Engine/Object.hpp"

struct Vector;

class SphereObject
	: public Object
{
public:
							SphereObject(Vector position, Material material, double radius);
							~SphereObject() override = default;

// Object i/f:
protected:
	IntersectionDistances	intersectWith(const Ray& ray) const override;
	Vector					normalAt(const Vector& position) const override;
	Color					colorAt(const Scene& scene, const Ray& ray) const override;

private:
	double					m_radius;
};
