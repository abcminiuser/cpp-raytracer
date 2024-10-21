#include "SolidTexture.hpp"

#include "Engine/Texture.hpp"

SolidTexture::SolidTexture(const Color& color)
	: Texture(1, 1, Palette::kWhite, Interpolation::NearestNeighbor)
	, m_color(color)
{

}

Color SolidTexture::colorAt(size_t x, size_t y) const
{
	return m_color;
}
