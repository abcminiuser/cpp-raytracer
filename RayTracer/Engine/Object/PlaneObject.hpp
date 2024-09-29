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
	double						intersectWith(const Ray& ray) const override;
	Vector						normalAt(const Vector& position) const override;
	Color						colorAt(const Scene& scene, const Ray& ray) const override;

private:
	std::shared_ptr<Texture>	m_texture;
	double						m_textureScaleFactor;
	Vector						m_normal;
};
