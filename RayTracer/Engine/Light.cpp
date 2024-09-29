#include "Engine/Light.hpp"

Light::Light(const Vector& position, const Color& color)
	: m_position(position)
	, m_color(color)
{

}

Color Light::illuminate(const Color& objectColor, double brightness) const
{
	return objectColor.multiply(m_color).scale(brightness);
}
