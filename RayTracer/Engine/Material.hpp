#pragma once

class Ray;

struct Vector;

class Material
{
public:
	virtual Ray	scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal) = 0;
};
