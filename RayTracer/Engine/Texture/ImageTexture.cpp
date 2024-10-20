#include "ImageTexture.hpp"

#include "Engine/Texture.hpp"

#include <cmath>
#include <stdexcept>

ImageTexture::ImageTexture(size_t width, size_t height, const uint32_t* pixels, Color multiplier)
	: m_width(width)
	, m_height(height)
	, m_pixels(&pixels[0], &pixels[width * height])
	, m_multiplier(multiplier)
{
	if (! m_width || ! m_height || ! pixels)
		throw std::runtime_error("Image texture create with invalid image parameters.");
}

Color ImageTexture::colorAt(double u, double v) const
{
	size_t x = static_cast<size_t>(u * (m_width - 1));
	size_t y = static_cast<size_t>((1 - v) * (m_height - 1));

	return Color::FromPackedRGBA(m_pixels[(y * m_width) + x]).multiply(m_multiplier);
}
