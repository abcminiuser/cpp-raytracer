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
	double						intersectWith(const Ray& ray) const override;
	Vector						normalAt(const Vector& position) const override;
	Color						colorAt(const Scene& scene, const Ray& ray) const override;

private:
	std::shared_ptr<Texture>	m_texture;
	double						m_radius;
	double						m_diameter;
};
