#include "DielectricMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <cmath>

namespace
{
	constexpr double kAirRefractionIndex = 1.0;
}

DielectricMaterial::DielectricMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals, double refractionIndex)
	: Material(std::move(texture), std::move(normals))
	, m_refractionIndex(refractionIndex)
{

}

std::optional<Ray> DielectricMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)
{
	// Refract if possible, otherwise reflect.
	auto scatterDirection = refract(incident, normal, kAirRefractionIndex, m_refractionIndex).value_or(reflect(incident, normal));

	attenuation = m_texture->sample(uv.x(), uv.y());
	return Ray(position, scatterDirection);
}
