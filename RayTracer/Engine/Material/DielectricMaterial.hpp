#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>
#include <optional>

class Texture;

class DielectricMaterial final
	: public Material
{
public:
							DielectricMaterial(std::shared_ptr<Texture> texture, double refractionIndex);
							~DielectricMaterial() = default;

// Material i/f:
public:
	std::optional<Ray>		scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& albedo) override;

private:
	double					m_refractionIndex;
};