#include "ReflectiveMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>

namespace
{
	Vector Reflect(const Vector& incident, const Vector& normal)
	{
		return incident - (normal * 2 * incident.dotProduct(normal));
	}
}

ReflectiveMaterial::ReflectiveMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals, double polish)
	: Material(std::move(texture), std::move(normals))
	, m_scuff(std::clamp(1 - polish, 0.0, 1.0))
{

}

std::optional<Ray> ReflectiveMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)
{
	// Reflect the incidence ray along the surface normal.
	auto reflectionDirection = Reflect(incident, normal).unit();

	// Perturb the reflected ray randomly, based on how (un-)polished this material is.
	if (m_scuff)
		reflectionDirection = (reflectionDirection + (VectorUtils::RandomUnitVector() * m_scuff)).unit();

	// If the perturbed ray is now pointing into the surface, absorb it.
	if (reflectionDirection.dotProduct(normal) < 0)
		return std::nullopt;

	attenuation = m_texture->sample(uv.x(), uv.y());
	return Ray(position, reflectionDirection);
}
