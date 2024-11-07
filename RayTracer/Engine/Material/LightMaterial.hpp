#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>

class Texture;

class LightMaterial final
	: public Material
{
public:
							LightMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals);
							~LightMaterial() override = default;

// Material i/f:
public:
	Color					emit(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv) override;
};
