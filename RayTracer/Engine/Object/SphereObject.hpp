#pragma once

#include "Engine/Material.hpp"
#include "Engine/Object.hpp"

#include <memory>

class Texture;

struct Vector;

class SphereObject
	: public Object
{
public:
				SphereObject(const Vector& position, double radius, std::shared_ptr<Texture> texture, const Material& material);
				~SphereObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Color& color) const override;

private:
	Color		colorAt(const Vector& position, const Vector& normal) const;

private:
	double		m_diameter;
};
