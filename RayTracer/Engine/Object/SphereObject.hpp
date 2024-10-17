#pragma once

#include "Engine/Object.hpp"

#include <memory>

class Material;

struct Vector;

class SphereObject
	: public Object
{
public:
				SphereObject(const Vector& position, const Vector& rotation, std::shared_ptr<Material> material, double radius);
				~SphereObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Vector& uv) const override;

private:
	Vector		uvAt(const Vector& position, const Vector& normal) const;

private:
	double		m_diameter;
};
