#include "Engine/Material.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"

Material::Material(std::shared_ptr<Texture> texture)
	: m_texture(std::move(texture))
{

}

Color Material::illuminate(const Scene& scene, const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal, const Vector& uv, uint32_t rayDepth)
{
	auto color = m_texture ? m_texture->colorAt(uv.x(), uv.y()) : Palette::kBlack;

	if (! scene.lighting)
		return color;

	auto scatterRay = scatter(sourceRay, hitPosition, hitNormal);
	return color.multiply(scatterRay.trace(scene, rayDepth + 1));
}
