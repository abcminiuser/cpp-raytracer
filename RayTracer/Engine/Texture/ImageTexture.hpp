#pragma once

#include "Engine/Color.hpp"
#include "Engine/Texture.hpp"

#include <stdint.h>
#include <stddef.h>
#include <vector>

class ImageTexture final
	: public Texture
{
public:
							ImageTexture(size_t width, size_t height, const uint32_t* pixels, Color multiplier = Color(1, 1, 1));
							~ImageTexture() override = default;

// Texture i/f:
public:
	Color					colorAt(double u, double v) const override;

private:
	size_t					m_width;
	size_t					m_height;
	std::vector<uint32_t>	m_pixels;
	Color					m_multiplier;
};
