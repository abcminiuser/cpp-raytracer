#pragma once

#include "Engine/Object.hpp"

#include <memory>

class Material;

struct Vector;

class PlaneObject final
	: public Object
{
public:
				PlaneObject(const Transform& transform, std::shared_ptr<Material> material);
				~PlaneObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const override;

private:
	Vector		uvAt(const Vector& position, const Vector& normal) const;

private:
	Vector		m_uvScale;
};
