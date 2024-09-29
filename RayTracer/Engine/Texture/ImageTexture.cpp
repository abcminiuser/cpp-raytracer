#include "ImageTexture.hpp"

#include "Engine/Texture.hpp"

#include <stdexcept>

ImageTexture::ImageTexture(size_t width, size_t height, const uint32_t* pixels)
	: m_width(width)
	, m_height(height)
	, m_pixels(&pixels[0], &pixels[width * height])
{
	if (! m_width || ! m_height || ! pixels)
		throw std::runtime_error("Image texture create with invalid image parameters.");
}

Color ImageTexture::colorAt(double u, double v) const
{
	const size_t x = static_cast<size_t>(u * m_width);
	const size_t y = (m_height - 1) - static_cast<size_t>(v * (m_height - 1));

	return Color(m_pixels.at((y * m_width) + x));
}
