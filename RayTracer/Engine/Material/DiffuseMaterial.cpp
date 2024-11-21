#include "DiffuseMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <numbers>

DiffuseMaterial::DiffuseMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals)
	: Material(std::move(texture), std::move(normals))
{

}

std::optional<Ray> DiffuseMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation, double& pdf)
{
	// Scatter across the unit hemisphere around the surface normal.
	auto scatterDirection = normal + VectorUtils::RandomUnitVector();
	if (scatterDirection.lengthSquared() < 1e-10)
		scatterDirection = normal;

	attenuation = m_texture->sample(uv.x(), uv.y());
	pdf = 1 / (2 * std::numbers::pi);
	return Ray(position, scatterDirection.unit());
}

double DiffuseMaterial::scatterPdf(const Vector& incident, const Vector& position, const Vector& normal, const Vector& scatteredDirection)
{
	 auto cosTheta = normal.dotProduct(scatteredDirection);
	 return cosTheta < 0 ? 0 : cosTheta / std::numbers::pi;
}
