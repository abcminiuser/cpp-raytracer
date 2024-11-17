#include "DielectricMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

DielectricMaterial::DielectricMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals, double refractionIndex)
	: Material(std::move(texture), std::move(normals))
	, m_refractionIndex(refractionIndex)
{

}

std::optional<Ray> DielectricMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)
{
	std::optional<Vector> refractionDirection;

	// Check if we're approaching from the front (entering)  or rear (exiting) side of the surface.
	if (normal.dotProduct(incident) < 0)
		refractionDirection = refract(incident, normal, 1.0 / m_refractionIndex);
	else
		refractionDirection = refract(incident, normal.inverted(), m_refractionIndex);

	// Refract if possible, otherwise reflect.
	auto scatterDirection = refractionDirection.value_or(reflect(incident, normal));

	attenuation = m_texture->sample(uv.x(), uv.y());
	return Ray(position, scatterDirection);
}
