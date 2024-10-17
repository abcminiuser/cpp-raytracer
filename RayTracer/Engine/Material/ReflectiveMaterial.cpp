#include "ReflectiveMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

ReflectiveMaterial::ReflectiveMaterial(std::shared_ptr<Texture> texture)
	: Material(std::move(texture))
{

}

std::optional<Ray> ReflectiveMaterial::scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal)
{
	return Ray(hitPosition, sourceRay.direction().subtract(hitNormal.scale(2 * sourceRay.direction().dotProduct(hitNormal))).unit());
}
