#pragma once

#include "Engine/BoundingBox.hpp"
#include "Engine/Color.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Vector.hpp"

#include <cstdint>
#include <limits>
#include <memory>

class Material;
class Ray;

struct Scene;

class Object
{
public:
	static inline constexpr double kComparisonThreshold		= 1e-10;

									Object(const BoundingBox& boundingBox, const Transform& transform, std::shared_ptr<Material> material);
	virtual							~Object() = default;

	const BoundingBox&				boundingBox() const { return m_boundingBox; }
	double							intersect(const Ray& ray) const;
	Color							illuminate(const Scene& scene, const Ray& ray, const Vector& position, uint32_t rayDepth) const;

protected:
	virtual double					intersectWith(const Ray& ray) const = 0;
	virtual void					getIntersectionProperties(const Vector& direction, const Vector& position, Vector& normal, Vector& tangent, Vector& bitangent, Vector& uv) const = 0;

private:
	const BoundingBox				m_boundingBox;
	const Transform					m_transform;
	const std::shared_ptr<Material>	m_material;
};
