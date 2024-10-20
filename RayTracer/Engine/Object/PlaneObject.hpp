#pragma once

#include "Engine/Object.hpp"

#include <memory>

class Material;

struct Vector;

class PlaneObject final
	: public Object
{
public:
				PlaneObject(std::shared_ptr<Material> material, const Vector& normal, double distance, double uvScaleFactor);
				~PlaneObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const override;

private:
	Vector		uvAt(const Vector& position, const Vector& normal) const;

private:
	Vector		m_normal;
	double		m_uvScaleFactor;
};
