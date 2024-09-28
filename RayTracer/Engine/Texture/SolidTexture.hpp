#pragma once

#include "Engine/Texture.hpp"

class SolidTexture
	: public Texture
{
public:
			SolidTexture(Color color)
				: m_color(color)
	{

	}

// Texture i.f:
public:
	Color	colorAt(double u, double v) const override
	{
		return m_color;
	}

private:
	Color	m_color;
};
