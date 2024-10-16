#include "ReflectiveMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>

Ray ReflectiveMaterial::scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal)
{
	return Ray(hitPosition, sourceRay.direction().subtract(hitNormal.scale(2 * sourceRay.direction().dotProduct(hitNormal))).unit());
}
