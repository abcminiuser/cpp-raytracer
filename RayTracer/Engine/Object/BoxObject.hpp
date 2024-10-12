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
	void						getIntersectionProperties(const Vector& position, Vector& normal, Color& color) const override;

private:
	Vector						normalAt(const Vector& position) const;
	Color						colorAt(const Vector& position, const Vector& normal) const;

private:
	Vector						m_size;
	std::shared_ptr<Texture>	m_texture;
};
