#pragma once

#include "Engine/Color.hpp"
#include "Engine/Texture.hpp"

#include <cstdint>
#include <cstddef>
#include <vector>

class ImageTexture final
	: public Texture
{
public:
							ImageTexture(size_t width, size_t height, const Color& multiplier, Interpolation interpolation, const uint32_t* pixels);
							~ImageTexture() override = default;

// Texture i/f:
public:
	Color					colorAt(size_t x, size_t y) const override;

private:
	std::vector<uint32_t>	m_pixels;
	Color					m_multiplier;
};
