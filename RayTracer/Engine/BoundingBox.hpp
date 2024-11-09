#pragma once

#include "Engine/Vector.hpp"

#include <array>

class Ray;

class BoundingBox
{
public:
	constexpr						BoundingBox() = default;

	constexpr						BoundingBox(const Vector& lower, const Vector& upper)
		: m_isSet(true)
		, m_lower(lower)
		, m_upper(upper)
	{

	}

	const Vector&					lower() const	{ return m_lower; }
	const Vector&					upper() const	{ return m_upper; }
	constexpr Vector				size() const	{ return m_upper - m_lower; }

	constexpr std::array<Vector, 8>	points() const
	{
		return
			{
				Vector(m_lower.x(), m_lower.y(), m_lower.z()),
				Vector(m_lower.x(), m_lower.y(), m_upper.z()),
				Vector(m_lower.x(), m_upper.y(), m_lower.z()),
				Vector(m_lower.x(), m_upper.y(), m_upper.z()),
				Vector(m_upper.x(), m_lower.y(), m_lower.z()),
				Vector(m_upper.x(), m_lower.y(), m_upper.z()),
				Vector(m_upper.x(), m_upper.y(), m_lower.z()),
				Vector(m_upper.x(), m_upper.y(), m_upper.z()),
			};
	}

	void							include(const Vector& point);

	double							intersect(const Ray& ray) const;

	bool							intersects(const BoundingBox& boundingBox) const;
	bool							contains(const Vector& point) const;

private:
	bool					m_isSet = false;
	Vector					m_lower;
	Vector					m_upper;
};
