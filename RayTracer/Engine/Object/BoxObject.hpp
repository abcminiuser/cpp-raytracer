#pragma once

#include "Engine/Object.hpp"

#include <memory>

class Material;

struct Vector;

class BoxObject final
	: public Object
{
public:
				BoxObject(const Vector& position, const Vector& rotation, std::shared_ptr<Material> material, const Vector& size);
				~BoxObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Vector& uv) const override;

private:
	Vector		normalAt(const Vector& position) const;
	Vector		uvAt(const Vector& position, const Vector& normal) const;

private:
	Vector		m_size;
};
