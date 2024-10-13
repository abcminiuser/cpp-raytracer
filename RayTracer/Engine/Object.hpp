#pragma once

#include "Engine/Color.hpp"
#include "Engine/Material.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Vector.hpp"

#include <limits>
#include <memory>
#include <stdint.h>

class Ray;
class Texture;

struct Scene;

class Object
{
public:
	static inline constexpr double kNoIntersection			= std::numeric_limits<double>::max();
	static inline constexpr double kComparisonThreshold		= std::numeric_limits<double>::epsilon() * 100;

								Object(const Vector& position, const Vector& rotation, std::shared_ptr<Texture> texture, const Material& material);
	virtual						~Object() = default;

	double						intersect(const Ray& ray) const;
	Color						illuminate(const Scene& scene, const Vector& position, const Ray& ray, uint32_t rayDepth) const;

protected:
	virtual double				intersectWith(const Ray& ray) const = 0;
	virtual void				getIntersectionProperties(const Vector& position, Vector& normal, Color& color) const = 0;

private:
	Matrix<3, 3>				makeRotationMatrix(const Vector& rotation) const;
	Vector						rotate(const Vector& vector, const Matrix<3, 3>& rotation) const;

protected:
	std::shared_ptr<Texture>	m_texture;
	Material					m_material;

private:
	Vector						m_position;

	Matrix<3, 3>				m_rotationMatrix;
	Matrix<3, 3>				m_rotationMatrixInverse;
};
