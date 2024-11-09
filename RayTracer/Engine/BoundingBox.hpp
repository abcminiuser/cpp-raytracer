#pragma once

#include "Engine/Vector.hpp"

class Ray;

class BoundingBox
{
public:
	constexpr			BoundingBox()
		: BoundingBox(StandardVectors::kZero, StandardVectors::kZero)
	{

	}

	constexpr			BoundingBox(const Vector& lower, const Vector& upper)
		: m_lower(lower)
		, m_upper(upper)
	{

	}

	constexpr void		setLower(const Vector& lower)	{ m_lower = lower; }
	constexpr void		setUpper(const Vector& upper)	{ m_upper = upper; }

	const Vector&		lower() const					{ return m_lower; }
	const Vector&		upper() const					{ return m_upper; }
	constexpr Vector	size() const					{ return m_upper - m_lower; }

	double				intersect(const Ray& ray) const;

	bool				intersects(const BoundingBox& boundingBox) const;
	bool				contains(const Vector& point) const;

private:
	Vector				m_lower;
	Vector				m_upper;
};
