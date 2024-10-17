#pragma once

#include <memory>

class Ray;
class Texture;

struct Color;
struct Scene;
struct Vector;

class Material
{
public:
									Material(std::shared_ptr<Texture> texture);
	virtual							~Material() = default;

	Color							illuminate(const Scene& scene, const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal, const Vector& uv, uint32_t rayDepth);

public:
	virtual Ray						scatter(const Ray& sourceRay, const Vector& hitPosition, const Vector& hitNormal) = 0;

protected:
	const std::shared_ptr<Texture>	m_texture;
};
