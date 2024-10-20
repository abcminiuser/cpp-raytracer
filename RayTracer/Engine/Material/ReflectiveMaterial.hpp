#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>
#include <optional>

class Texture;

class ReflectiveMaterial final
	: public Material
{
public:
							ReflectiveMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals, double polish);
							~ReflectiveMaterial() = default;

// Material i/f:
public:
	std::optional<Ray>		scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation) override;

private:
	double					m_scuff;
};
