#include "SolidTexture.hpp"

#include "Engine/Texture.hpp"

SolidTexture::SolidTexture(const Color& color)
	: m_color(color)
{

}

Color SolidTexture::colorAt(double u, double v) const
{
	return m_color;
}
