#pragma once

#include "Engine/BoundingBox.hpp"
#include "Engine/Color.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vector.hpp"

#include <limits>
#include <memory>
#include <stdint.h>

class Material;
class Ray;

struct Scene;

class Object
{
public:
	static inline constexpr double kComparisonThreshold		= 1e-10;

									Object(const Vector& position, const Vector& rotation, const Vector& scale, std::shared_ptr<Material> material);
	virtual							~Object() = default;

	double							intersect(const Ray& ray) const;
	Color							illuminate(const Scene& scene, const Ray& ray, double distance, uint32_t rayDepthRemaining) const;

protected:
	virtual double					intersectWith(const Ray& ray) const = 0;
	virtual void					getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const = 0;

protected:
	const std::shared_ptr<Material>	m_material;

private:
	Vector							m_position;
	Vector							m_rotation;
	Vector							m_scale;

	Matrix<4, 4>					m_transform;
	Matrix<4, 4>					m_inverseTransform;
};
