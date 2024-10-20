#pragma once

#include "Engine/Color.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <memory>
#include <optional>

class Texture;

struct Scene;

class Material
{
public:
									Material(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals);
	virtual							~Material() = default;

	Vector							mapNormal(const Vector& normal, const Vector& tangent, const Vector& bitangent, const Vector& uv) const;
	Color							illuminate(const Scene& scene, const Ray& sourceRay, const Vector& position, const Vector& normal, const Vector& uv, uint32_t rayDepthRemaining);

public:
	virtual Color					emit(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv)						{ return Palette::kBlack; }
	virtual std::optional<Ray>		scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation)	{ return std::nullopt; }


protected:
	const std::shared_ptr<Texture>	m_texture;
	const std::shared_ptr<Texture>	m_normals;
};
