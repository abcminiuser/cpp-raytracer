#include "DielectricMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

#include <cmath>

namespace
{
	constexpr double kAirRefractionIndex = 1.0;

	std::optional<Vector> Refract(const Vector& incident, const Vector& normal, double n1, double n2)
	{
		// https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf

		double n = n1 / n2;
		double cosI = incident.dotProduct(normal);
		double sinT2 = (n * n) * (1.0 - (cosI * cosI));

		if (sinT2 > 1.0)
			return std::nullopt;

		double cosT = std::sqrt(1.0 - sinT2);
		return incident.scale(n).add(normal.scale(n * cosI - cosT)).unit();
	}
}

DielectricMaterial::DielectricMaterial(std::shared_ptr<Texture> texture, double refractionIndex)
	: Material(std::move(texture))
	, m_refractionIndex(refractionIndex)
{

}

std::optional<Ray> DielectricMaterial::scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)
{
	auto refractionDirection = Refract(incident, normal, kAirRefractionIndex, m_refractionIndex);
	if (! refractionDirection)
		return std::nullopt;

	attenuation = m_texture->colorAt(uv.x(), uv.y());
	return Ray(position, refractionDirection.value());
}
