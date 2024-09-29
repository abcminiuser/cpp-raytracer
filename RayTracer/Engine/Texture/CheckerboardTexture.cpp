#include "CheckerboardTexture.hpp"

#include "Engine/Texture.hpp"

CheckerboardTexture::CheckerboardTexture(const Color& color1, const Color& color2, uint8_t rowsCols)
	: m_color1(color1)
	, m_color2(color2)
	, m_rowsCols(rowsCols)
{

}

Color CheckerboardTexture::colorAt(double u, double v) const
{
	const auto row = static_cast<uint8_t>(u * m_rowsCols);
	const auto col = static_cast<uint8_t>(v * m_rowsCols);

	return ((row ^ col) & 1) ? m_color1 : m_color2;
}
