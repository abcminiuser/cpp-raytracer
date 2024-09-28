#pragma once

#include "Engine/Texture.hpp"

#include <stdint.h>

class CheckerboardTexture
	: public Texture
{
public:
			CheckerboardTexture(Color color1, Color color2, uint8_t rowsCols)
				: m_color1(color1)
				, m_color2(color2)
				, m_rowsCols(rowsCols)
	{

	}

// Texture i/f:
public:
	Color	colorAt(double u, double v) const override
	{
		const auto row = static_cast<uint8_t>(u * m_rowsCols);
		const auto col = static_cast<uint8_t>(v * m_rowsCols);

		return ((row ^ col) & 1) ? m_color1 : m_color2;
	}

private:
	Color	m_color1;
	Color	m_color2;
	uint8_t	m_rowsCols;
};
