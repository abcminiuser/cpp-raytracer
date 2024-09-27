#pragma once

#include <stdint.h>

#include "Color.hpp"
#include "Vector.hpp"

struct Light
{
public:
					Light(Vector position, Color color);

	const Vector&	position() const { return m_position; }

	Color			illuminate(const Color& objectColor, const Vector& position, float brightness) const;

private:
	Vector			m_position;
	Color			m_color;
};
