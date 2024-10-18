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
							DielectricMaterial(double refractionIndex);
							~DielectricMaterial() = default;

// Material i/f:
public:
	std::optional<Ray>		scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal, const Vector& uv, Color& albedo) override;

private:
	double					m_refractionIndex;
};
