#include "ReflectiveMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

namespace
{
	Vector Reflect(const Vector& incident, const Vector& normal)
	{
		return incident.subtract(normal.scale(2 * incident.dotProduct(normal)));
	}
}

ReflectiveMaterial::ReflectiveMaterial(std::shared_ptr<Texture> texture, double polish)
	: Material(std::move(texture))
	, m_scuff(std::clamp(1 - polish, 0.0, 1.0))
{

}

std::optional<Ray> ReflectiveMaterial::scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal)
{
	// Reflect the incidence ray along the surface normal.
	auto reflectionDirection = Reflect(sourceRay.direction(), hitNormal).unit();

	// Perturb the reflected ray randomly, based on how (un-)polished this material is.
	if (m_scuff)
		reflectionDirection = reflectionDirection.add(VectorUtils::RandomUnitVector().scale(m_scuff)).unit();

	// If the perturbed ray is now pointing into the surface, absorb it.
	if (reflectionDirection.dotProduct(hitNormal) < 0)
		return std::nullopt;

	return Ray(hitPosition, reflectionDirection);
}
