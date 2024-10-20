#include "ImageTexture.hpp"

#include "Engine/Texture.hpp"

#include <cmath>
#include <stdexcept>

ImageTexture::ImageTexture(size_t width, size_t height, Interpolation interpolation, const uint32_t* pixels, Color multiplier)
	: Texture(width, height, interpolation)
	, m_pixels(&pixels[0], &pixels[width * height])
	, m_multiplier(multiplier)
{
	if (! width || ! height || ! pixels)
		throw std::runtime_error("Image texture create with invalid image parameters.");
}

Color ImageTexture::colorAt(size_t x, size_t y) const
{
	y = (m_height - 1) - y;
	return Color::FromPackedRGBA(m_pixels[(y * m_width) + x]).multiply(m_multiplier);
}
