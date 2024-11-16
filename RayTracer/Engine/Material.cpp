#include "Engine/Material.hpp"

#include "Engine/Matrix.hpp"
#include "Engine/Random.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"

#include <algorithm>

namespace
{
	const auto kDefaultTexture = std::make_shared<CheckerboardTexture>(Texture::Interpolation::NearestNeighbor, Palette::kWhite, Palette::kWhite * .5, 15);
}

Material::Material(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals)
	: m_texture(texture ? std::move(texture) : kDefaultTexture)
	, m_normals(std::move(normals))
{

}

Vector Material::mapNormal(const Vector& normal, const Vector& tangent, const Vector& bitangent, const Vector& uv) const
{
	if (! m_normals)
		return normal;

	// https://medium.com/@muhammedcan.erbudak/ray-tracing-from-scratch-texture-normal-bump-mapping-22ece96038bf

	const auto mappedNormalColor = m_normals->sample(uv.x(), uv.y());

	const Matrix<3, 1> input
		({
			mappedNormalColor.red() * 2 - 1,
			mappedNormalColor.green() * 2 - 1,
			mappedNormalColor.blue() * 2 - 1
		});

	const Matrix<3, 3> mappingMatrix
		({
			tangent.x(), bitangent.x(), normal.x(),
			tangent.y(), bitangent.y(), normal.y(),
			tangent.z(), bitangent.z(), normal.z()
		});

	const Matrix<3, 1> output = mappingMatrix * input;

	return Vector(output(0, 0), output(0, 1), output(0, 2)).unit();
}

Color Material::illuminate(const Scene& scene, const Ray& sourceRay, const Vector& position, const Vector& normal, const Vector& uv, uint32_t rayDepth)
{
	Color finalColor = emit(sourceRay.direction(), position, normal, uv);

	Color attenuation;
	if (auto scatterRay = scatter(sourceRay.direction(), position, normal, uv, attenuation))
	{
		// Russian roulette termination; once we've reached at least three
		// bounces, start pruning rays based on the survival probability.
		if (rayDepth >= 3)
		{
			// Never allow a survival probability of 1.0, or we could potentially
			// never terminate.
			const double survivalProbability = std::min(attenuation.average(), 0.99);

			// Check if we need to terminate this ray, or boost it based on the
			// survival probability.
			if (Random::UnsignedNormal() > survivalProbability)
				attenuation = Color();
			else
				attenuation *= 1 / survivalProbability;
		}

		if (attenuation != Color())
			finalColor += attenuation * scatterRay->trace(scene, rayDepth);
	}

	return finalColor;
}
