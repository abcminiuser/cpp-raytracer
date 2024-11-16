#pragma once

#include "Engine/Color.hpp"
#include "Engine/Texture.hpp"

#include <stddef.h>

class SolidTexture final
	: public Texture
{
public:
	explicit	SolidTexture(const Color& color);
				~SolidTexture() override = default;

// Texture i/f:
public:
	Color		colorAt(size_t x, size_t y) const override;

private:
	Color		m_color;
};
