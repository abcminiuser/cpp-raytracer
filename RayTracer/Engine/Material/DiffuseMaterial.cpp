#include "DiffuseMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

DiffuseMaterial::DiffuseMaterial(std::shared_ptr<Texture> texture)
	: Material(std::move(texture))
{

}

std::optional<Ray> DiffuseMaterial::scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal, const Vector& uv, Color& attenuation)
{
	// Find a random unit vector in the same hemisphere as our surface normal. Easiest to
	// first generate a random unit vector, then invert it if it landed on the wrong side.
	Vector randomHemisphereDirection = VectorUtils::RandomUnitVector();
	if (randomHemisphereDirection.dotProduct(hitNormal) < 0)
		randomHemisphereDirection = randomHemisphereDirection.invert();

	if (randomHemisphereDirection.lengthSquared() < 1e-10)
		randomHemisphereDirection = hitNormal;

	// Lambertian diffusion; always scatter.
	attenuation = m_texture->colorAt(uv.x(), uv.y());
	return Ray(hitPosition, hitNormal.add(randomHemisphereDirection).unit());
}
