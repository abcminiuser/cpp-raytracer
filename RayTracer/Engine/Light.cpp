#include "Light.hpp"

Light::Light(Vector position, Color color)
	: m_position(std::move(position))
	, m_color(std::move(color))
{}

Color Light::illuminate(const Color& objectColor, const Vector& position, float brightness)
{
	return objectColor.multiply(m_color).scale(brightness);
}
