#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>
#include <optional>

class Texture;

class DiffuseMaterial final
	: public Material
{
public:
							DiffuseMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals);
							~DiffuseMaterial() = default;

// Material i/f:
public:
	std::optional<Ray>		scatter(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv, Color& albedo) override;
};
