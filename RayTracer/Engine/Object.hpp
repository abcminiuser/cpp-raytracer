#pragma once

#include "Engine/BoundingBox.hpp"
#include "Engine/Color.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Ray.hpp"
#include "Engine/Vector.hpp"

#include <limits>
#include <memory>
#include <stdint.h>

class Material;

struct Scene;

class Object
{
public:
	static constexpr auto kNoIntersection = Ray::kNoIntersection;

	static inline constexpr double kComparisonThreshold		= 1e-10;

									Object(const Vector& position, const Vector& rotation, std::shared_ptr<Material> material);
	virtual							~Object() = default;

	double							intersect(const Ray& ray) const;
	Color							illuminate(const Scene& scene, const Ray& ray, const Vector& position, uint32_t rayDepthRemaining) const;

protected:
	virtual double					intersectWith(const Ray& ray) const = 0;
	virtual void					getIntersectionProperties(const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const = 0;

private:
	Matrix<3, 3>					makeRotationMatrix(const Vector& rotation) const;
	Vector							rotate(const Vector& vector, const Matrix<3, 3>& rotation) const;

protected:
	const std::shared_ptr<Material>	m_material;

private:
	Vector							m_position;

	bool							m_hasRotation = false;
	Matrix<3, 3>					m_rotationMatrix;
	Matrix<3, 3>					m_rotationMatrixInverse;
};
