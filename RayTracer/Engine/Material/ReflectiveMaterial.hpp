#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>
#include <optional>

class Texture;

class ReflectiveMaterial
	: public Material
{
public:
							ReflectiveMaterial(std::shared_ptr<Texture> texture);
							~ReflectiveMaterial() = default;

// Material i/f:
public:
	std::optional<Ray>		scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal) override;
};
