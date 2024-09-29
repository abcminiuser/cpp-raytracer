#pragma once

#include "Engine/Material.hpp"
#include "Engine/Object.hpp"

struct Vector;

class PlaneObject
	: public Object
{
public:
							PlaneObject(const Vector& normal, double distance, const Material& material, double textureScaleFactor);
							~PlaneObject() override = default;

// Object i/f:
protected:
	double					intersectWith(const Ray& ray) const override;
	Vector					normalAt(const Vector& position) const override;
	Color					colorAt(const Scene& scene, const Ray& ray) const override;

private:
	Vector					m_normal;
	double					m_textureScaleFactor;
};
