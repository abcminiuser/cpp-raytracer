#include "Engine/Material.hpp"

#include "Engine/Matrix.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"

Material::Material(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals)
	: m_texture(texture ? std::move(texture) : std::make_shared<CheckerboardTexture>(Texture::Interpolation::NearestNeighbor, Palette::kWhite, Palette::kWhite * .5, 10))
	, m_normals(std::move(normals))
{

}

Vector Material::mapNormal(const Vector& normal, const Vector& tangent, const Vector& bitangent, const Vector& uv) const
{
	if (! m_normals)
		return normal;

	// https://medium.com/@muhammedcan.erbudak/ray-tracing-from-scratch-texture-normal-bump-mapping-22ece96038bf

	const auto mappedNormal = m_normals->sample(uv.x(), uv.y());
	const auto mappedNormalVector = Vector(mappedNormal.red() * 2 - 1, mappedNormal.green() * 2 - 1, mappedNormal.blue() * 2 - 1);

	const Matrix<3, 3> mappingMatrix
		({
			tangent.x(), bitangent.x(), normal.x(),
			tangent.y(), bitangent.y(), normal.y(),
			tangent.z(), bitangent.z(), normal.z()
		});

	return mappingMatrix.multiply(mappedNormalVector).unit();
}

Color Material::illuminate(const Scene& scene, const Ray& sourceRay, const Vector& position, const Vector& normal, const Vector& uv, uint32_t rayDepthRemaining)
{
	if (scene.maxRayDepth == 1)
		return m_texture->sample(uv.x(), uv.y());

	Color finalColor = emit(sourceRay.direction(), position, normal, uv);

	Color attenuation;
	if (auto scatterRay = scatter(sourceRay.direction(), position, normal, uv, attenuation))
		finalColor += attenuation * scatterRay->trace(scene, rayDepthRemaining);

	return finalColor;
}
