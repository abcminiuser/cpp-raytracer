#pragma once

#include <algorithm>
#include <stdint.h>
#include <cmath>

struct Vector
{
public:
	constexpr			Vector()
							: Vector(0, 0, 0)
	{}

	constexpr			Vector(float x, float y, float z)
		: m_x(x)
		, m_y(y)
		, m_z(z)
	{}

	constexpr float		squaredEuclidianDistance() const
	{
		if (! m_squaredEuclidianDistance)
			m_squaredEuclidianDistance = m_x * m_x + m_y * m_y + m_z * m_z;

		return m_squaredEuclidianDistance;
	}

	constexpr float		length() const
	{
		if (! m_length)
			m_length = std::sqrt(squaredEuclidianDistance());

		return m_length;
	}

	constexpr Vector	add(const Vector& other) const
	{
		return Vector(m_x + other.m_x, m_y + other.m_y, m_z + other.m_z);
	}

	constexpr Vector	subtract(const Vector& other) const
	{
		return Vector(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
	}

	constexpr Vector	multiply(const Vector& other) const
	{
		return Vector(m_x * other.m_x, m_y * other.m_y, m_z * other.m_z);
	}

	constexpr Vector	crossProduct(const Vector& other) const
	{
		return Vector(
			(m_y * other.m_z) - (m_z * other.m_y),
			(m_z * other.m_x) - (m_x * other.m_z),
			(m_x * other.m_y) - (m_y * other.m_x)
		);
	}

	constexpr float		dotProduct(const Vector& other) const
	{
		return (m_x * other.m_x) + (m_y * other.m_y) + (m_z * other.m_z);
	}

	constexpr Vector	scale(float factor) const
	{
		return Vector(m_x * factor, m_y * factor, m_z * factor);
	}

	constexpr Vector	invert() const
	{
		return Vector(-m_x, -m_y, -m_z);
	}

	constexpr Vector	unit() const
	{
		const auto l = length();
		return l ? scale(1 / l) : Vector();
	}

private:
	float				m_x = 0;
	float				m_y = 0;
	float				m_z = 0;

private:
	mutable float		m_squaredEuclidianDistance = 0;
	mutable float		m_length = 0;
};

namespace StandardVectors
{
	static inline constexpr auto kOrigin = Vector(0, 0, 0);
	static inline constexpr auto kUnitX = Vector(1, 0, 0);
	static inline constexpr auto kUnitY = Vector(0, 1, 0);
	static inline constexpr auto kUnitZ = Vector(0, 0, 1);
}