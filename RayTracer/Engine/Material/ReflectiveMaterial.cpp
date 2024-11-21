#include "ReflectiveMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>

ReflectiveMaterial::ReflectiveMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals, double polish)
	: Material(std::move(texture), std::move(normals))
	, m_scuff(std::clamp(1 - polish, 0.0, 1.0))
{

}

std::optional<Ray> ReflectiveMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation, double& pdf)
{
	// Reflect the incidence ray along the surface normal.
	auto scatterDirection = reflect(incident, normal);

	// Perturb the reflected ray randomly, based on how (un-)polished this material is.
	if (m_scuff)
		scatterDirection = (scatterDirection + (VectorUtils::RandomUnitVector() * m_scuff)).unit();

	// If the perturbed ray is now pointing into the surface, absorb it.
	if (scatterDirection.dotProduct(normal) < 0)
		return std::nullopt;

	attenuation = m_texture->sample(uv.x(), uv.y());
	pdf = 1;
	return Ray(position, scatterDirection);
}

double ReflectiveMaterial::scatterPdf(const Vector& incident, const Vector& position, const Vector& normal, const Vector& scatteredDirection)
{
	 return 1;
}
