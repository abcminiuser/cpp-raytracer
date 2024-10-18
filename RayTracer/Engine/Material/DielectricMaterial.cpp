#include "DielectricMaterial.hpp"

#include "Engine/Ray.hpp"
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

DielectricMaterial::DielectricMaterial(double refractionIndex)
	: Material(nullptr)
	, m_refractionIndex(refractionIndex)
{

}

std::optional<Ray> DielectricMaterial::scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal, const Vector& uv, Color& attenuation)
{
	auto refractionDirection = Refract(sourceRay.direction(), hitNormal, kAirRefractionIndex, m_refractionIndex);
	if (! refractionDirection)
		return std::nullopt;

	attenuation = Palette::kWhite;
	return Ray(hitPosition, refractionDirection.value());
}
