#pragma once

#include "Engine/Color.hpp"
#include "Engine/Texture.hpp"

#include <stdint.h>

class CheckerboardTexture final
	: public Texture
{
public:
			CheckerboardTexture(const Color& color1, const Color& color2, uint8_t rowsCols);
			~CheckerboardTexture() override = default;

// Texture i/f:
public:
	Color	colorAt(double u, double v) const override;

private:
	Color	m_color1;
	Color	m_color2;
	uint8_t	m_rowsCols;
};
