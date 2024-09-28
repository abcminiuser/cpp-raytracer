#pragma once

#include "Engine/Texture.hpp"

#include <stdint.h>
#include <vector>

class ImageTexture
	: public Texture
{
public:
							ImageTexture(size_t width, size_t height, const uint32_t* pixels)
								: m_width(width)
								, m_height(height)
								, m_pixels(&pixels[0], &pixels[width * height])
	{

	}

// Texture i/f:
public:
	Color					colorAt(double u, double v) const override
	{
		if (! m_width || ! m_height)
			return Palette::kWhite.scale(.5);

		const auto x = static_cast<size_t>(u * m_width);
		const auto y = static_cast<size_t>(v * (m_height - 1));

		return Color(m_pixels.at((y * m_width) + x));
	}

private:
	size_t					m_width;
	size_t					m_height;
	std::vector<uint32_t>	m_pixels;
};
