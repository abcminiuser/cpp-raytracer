#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

class ReflectiveMaterial
	: public Material
{
// Material i/f:
public:
	Ray		scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal) override;
};
