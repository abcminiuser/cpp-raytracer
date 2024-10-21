#include "CheckerboardTexture.hpp"

#include "Engine/Texture.hpp"

namespace
{
	constexpr auto kVirtualTextureSize = 64;
}

CheckerboardTexture::CheckerboardTexture(Interpolation interpolation, const Color& color1, const Color& color2, uint8_t rowsCols)
	: Texture(rowsCols * kVirtualTextureSize, rowsCols * kVirtualTextureSize, Palette::kWhite, interpolation)
	, m_color1(color1)
	, m_color2(color2)
{

}

Color CheckerboardTexture::colorAt(size_t x, size_t y) const
{
	// We use a larger (virtual) texture size, so that we don't end up with a
	// blurry mess when interpolation is applied.
	x /= kVirtualTextureSize;
	y /= kVirtualTextureSize;

	return ((x ^ y) & 1) ? m_color1 : m_color2;
}
