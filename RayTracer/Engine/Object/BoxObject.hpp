#pragma once

#include "Engine/Object.hpp"

#include <memory>

class Material;

struct Vector;

class BoxObject final
	: public Object
{
public:
				BoxObject(const Transform& transform, std::shared_ptr<Material> material);
				~BoxObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const override;

private:
	Vector		uvAt(const Vector& position, const Vector& normal) const;
};
