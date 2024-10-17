#include "Engine/Material.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"

#include <cassert>

Material::Material(std::shared_ptr<Texture> texture)
	: m_texture(std::move(texture))
{
	assert(m_texture);
}

Color Material::illuminate(const Scene& scene, const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal, const Vector& uv, uint32_t rayDepth)
{
	if (! scene.lighting)
		return m_texture->colorAt(uv.x(), uv.y());

	Color finalColor = emit(uv);

	auto scatterRay = scatter(sourceRay, hitPosition, hitNormal);
	if (scatterRay)
		finalColor = finalColor.add(m_texture->colorAt(uv.x(), uv.y()).multiply(scatterRay->trace(scene, rayDepth + 1)));

	return finalColor;
}
