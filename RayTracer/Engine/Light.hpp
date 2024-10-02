#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

struct Material;
struct Scene;

class Ray;

struct Light
{
public:
					Light(const Vector& position, const Color& color);

	Color			illuminate(const Scene& scene, const Vector& normal, const Color& color, const Ray& reflectionRay, const Material& material) const;

private:
	Vector			m_position;
	Color			m_color;
};
