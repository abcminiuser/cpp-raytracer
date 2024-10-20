#include "DiffuseMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

DiffuseMaterial::DiffuseMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals)
	: Material(std::move(texture), std::move(normals))
{

}

std::optional<Ray> DiffuseMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)
{
	// Find a random unit vector in the same hemisphere as our surface normal. Easiest to
	// first generate a random unit vector, then invert it if it landed on the wrong side.
	Vector randomHemisphereDirection = VectorUtils::RandomUnitVector();
	if (randomHemisphereDirection.dotProduct(normal) < 0)
		randomHemisphereDirection = randomHemisphereDirection.invert();

	if (randomHemisphereDirection.lengthSquared() < 1e-10)
		randomHemisphereDirection = normal;

	// Lambertian diffusion; always scatter.
	attenuation = m_texture->sample(uv.x(), uv.y());
	return Ray(position, normal.add(randomHemisphereDirection).unit());
}
