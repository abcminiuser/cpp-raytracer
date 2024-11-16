#pragma once

#include "Engine/Color.hpp"
#include "Engine/Texture.hpp"

#include <stddef.h>

class CheckerboardTexture final
	: public Texture
{
public:
			CheckerboardTexture(Interpolation interpolation, const Color& color1, const Color& color2, uint8_t rowsCols);
			~CheckerboardTexture() override = default;

// Texture i/f:
public:
	Color	colorAt(size_t x, size_t y) const override;

private:
	Color	m_color1;
	Color	m_color2;
};
