#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>
#include <optional>

class Texture;

class LightMaterial final
	: public Material
{
public:
							LightMaterial(std::shared_ptr<Texture> texture);
							~LightMaterial() = default;

// Material i/f:
public:
	Color					emit(const Vector& uv) override;
};
