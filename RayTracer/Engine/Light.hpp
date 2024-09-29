#pragma once

#include "Engine/Color.hpp"
#include "Engine/Vector.hpp"

#include <stdint.h>

struct Light
{
public:
					Light(Vector position, Color color);

	const Vector&	position() const { return m_position; }

	Color			illuminate(const Color& objectColor, double brightness) const;

private:
	Vector			m_position;
	Color			m_color;
};
