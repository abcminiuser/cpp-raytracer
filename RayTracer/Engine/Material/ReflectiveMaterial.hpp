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
							~ReflectiveMaterial() override = default;

// Material i/f:
public:
	std::optional<Ray>		scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& attenuation, double& pdf) override;
	double					scatterPdf(const Vector& incident, const Vector& position, const Vector& normal, const Vector& scatteredDirection) override;

private:
	double					m_scuff;
};
