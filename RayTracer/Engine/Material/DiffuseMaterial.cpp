#include "DiffuseMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <algorithm>
#include <random>

namespace
{
	Vector		RandomUnitVector()
	{
	    static std::mt19937 generator;
		static std::uniform_real_distribution<double> distribution(-1.0, 1.0);

	    for (;;)
	    {
			auto v = Vector(distribution(generator), distribution(generator), distribution(generator));
			if (v.lengthSquared() > 0.00001 && v.lengthSquared() < 1)
				return v.unit();
		}
	}
}

Ray DiffuseMaterial::scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal)
{
	// Find a random unit vector in the same hemisphere as our surface normal. Easiest to
	// first generate a random unit vector, then invert it if it landed on the wrong side.
	Vector randomHemisphereDirection = RandomUnitVector();
	if (randomHemisphereDirection.dotProduct(hitNormal) < 0)
		randomHemisphereDirection = randomHemisphereDirection.invert();

	if (randomHemisphereDirection.lengthSquared() < .00001)
		randomHemisphereDirection = hitNormal;

	// Lambertian diffusion; always scatter with fixed attenuation.
	return Ray(hitPosition, hitNormal.add(randomHemisphereDirection).unit());
}
