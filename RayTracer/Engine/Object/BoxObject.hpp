#pragma once

#include "Engine/Material.hpp"
#include "Engine/Object.hpp"

#include <memory>

class Texture;

struct Vector;

class BoxObject
	: public Object
{
public:
								BoxObject(const Vector& position, const Vector& size, std::shared_ptr<Texture> texture, const Material& material);
								~BoxObject() override = default;

// Object i/f:
protected:
	double						intersectWith(const Ray& ray) const override;
	Vector						normalAt(const Vector& position) const override;
	Color						colorAt(const Scene& scene, const Ray& ray) const override;

private:
	Vector						m_size;
	Vector						m_lowerCorner;
	Vector						m_upperCorner;
	std::shared_ptr<Texture>	m_texture;
};
