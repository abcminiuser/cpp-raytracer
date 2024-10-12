#pragma once

#include "Engine/Material.hpp"
#include "Engine/Object.hpp"

#include <memory>

class Texture;

struct Vector;

class PlaneObject
	: public Object
{
public:
				PlaneObject(const Vector& normal, double distance, std::shared_ptr<Texture> texture, double textureScaleFactor, const Material& material);
				~PlaneObject() override = default;

// Object i/f:
protected:
	double		intersectWith(const Ray& ray) const override;
	void		getIntersectionProperties(const Vector& position, Vector& normal, Color& color) const override;

private:
	Color		colorAt(const Vector& position, const Vector& normal) const;

private:
	double		m_textureScaleFactor;
	Vector		m_normal;
};
