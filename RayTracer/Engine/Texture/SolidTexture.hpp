#pragma once

#include "Engine/Color.hpp"
#include "Engine/Texture.hpp"

class SolidTexture final
	: public Texture
{
public:
			SolidTexture(const Color& color);
			~SolidTexture() override = default;

// Texture i/f:
public:
	Color	colorAt(size_t x, size_t y) const override;

private:
	Color	m_color;
};
