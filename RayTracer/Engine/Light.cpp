#include "Engine/Light.hpp"

#include "Material.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

#include <cmath>

namespace
{
	constexpr double kSpecularMultiplier = 32;
}

Light::Light(const Vector& position, const Color& color)
	: m_position(position)
	, m_color(color)
{

}

Color Light::illuminate(const Scene& scene, const Vector& normal, const Color& color, const Ray& reflectionRay, const Material& material) const
{
	const Vector	objectToLight		= m_position.subtract(reflectionRay.position());
	const Ray		objectToLightRay	= Ray(reflectionRay.position(), objectToLight.unit());

	for (const auto& o : scene.objects)
	{
		const double intersectionDistance = o->intersect(objectToLightRay);
		if (intersectionDistance <= objectToLight.length())
			return Palette::kBlack;
	}

	Color finalColor = Palette::kBlack;

	if (material.diffuse > 0)
	{
		double brightness = normal.dotProduct(objectToLightRay.direction());
		if (brightness > 0)
			finalColor = finalColor.add(color.multiply(m_color).scale(brightness * material.diffuse));
	}

	if (material.specular > 0)
	{
		double brightness = reflectionRay.direction().dotProduct(objectToLightRay.direction());
		if (brightness > 0)
		{
			brightness *= std::pow(brightness, kSpecularMultiplier * material.specular * material.specular);
			finalColor = finalColor.add(color.multiply(m_color).scale(brightness * material.specular));
		}
	}

	return finalColor;
}
