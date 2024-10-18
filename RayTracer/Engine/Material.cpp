#include "Engine/Material.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"

Material::Material(std::shared_ptr<Texture> texture)
	: m_texture(texture ? std::move(texture) : std::make_shared<CheckerboardTexture>(Palette::kWhite, Palette::kWhite.scale(.5), 10))
{

}

Color Material::illuminate(const Scene& scene, const Ray& sourceRay, const Vector& position, const Vector& normal, const Vector& uv, uint32_t rayDepthRemaining)
{
	if (! scene.lighting)
		return m_texture->colorAt(uv.x(), uv.y());

	Color finalColor = emit(sourceRay.direction(), position, normal, uv);

	Color attenuation;
	if (auto scatterRay = scatter(sourceRay.direction(), position, normal, uv, attenuation))
		finalColor = finalColor.add(attenuation.multiply(scatterRay->trace(scene, rayDepthRemaining)));

	return finalColor;
}
