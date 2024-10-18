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
	Color	colorAt(double u, double v) const override;

private:
	Color	m_color;
};
