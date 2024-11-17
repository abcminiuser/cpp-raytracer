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
	Vector direction = normal + VectorUtils::RandomUnitVector();
	if (direction.lengthSquared() < 1e-10)
		direction = normal;

	// Lambertian diffusion; always scatter.
	attenuation = m_texture->sample(uv.x(), uv.y());
	return Ray(position, direction.unit());
}
