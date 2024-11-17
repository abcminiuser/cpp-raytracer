#include "DielectricMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

namespace
{
	double Reflectance(double cosAngle, double refractionIndex)
	{
		// Use Schlick's approximation for reflectance.

		double r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);

		r0 *= r0;

		return r0 + (1 - r0) * std::pow(1 - cosAngle, 5);
	}
}

DielectricMaterial::DielectricMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals, double refractionIndex)
	: Material(std::move(texture), std::move(normals))
	, m_refractionIndex(refractionIndex)
{

}

std::optional<Ray> DielectricMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)
{
	double refractionIndex;
	Vector refractionNormal;

	// Check if we're approaching from the front (entering) or rear (exiting) side of the surface.
	if (incident.dotProduct(normal) > 0)
	{
		// Rear side
		refractionIndex		= m_refractionIndex;
		refractionNormal	= normal.inverted();
	}
	else
	{
		// Front side
		refractionIndex		= 1.0 / m_refractionIndex;
		refractionNormal	= normal;
	}

	std::optional<Vector> refractionDirection;

	double cosAngle = std::min(incident.inverted().dotProduct(refractionNormal), 1.0);
	if (Reflectance(cosAngle, refractionIndex) <= Random::UnsignedNormal())
		refractionDirection = refract(incident, refractionNormal, refractionIndex);

	// Refract if possible, otherwise reflect.
	auto scatterDirection = refractionDirection.value_or(reflect(incident, normal));

	attenuation = m_texture->sample(uv.x(), uv.y());
	return Ray(position, scatterDirection);
}
