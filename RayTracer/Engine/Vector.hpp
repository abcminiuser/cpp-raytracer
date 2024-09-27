#pragma once

#include <algorithm>
#include <stdint.h>
#include <optional>
#include <cmath>

struct Vector
{
public:
	constexpr		 Vector()
						: Vector(0, 0, 0)
	{}

	constexpr 		Vector(float px, float py, float pz)
		: x(px)
		, y(py)
		, z(pz)
	{}

	constexpr float length() const
	{
		if (! m_length)
			m_length = std::sqrt(x * x + y * y + z * z);

		return *m_length;
	}

	constexpr Vector add(const Vector& other) const
	{
		return Vector(x + other.x, y + other.y, z + other.z);
	}

	constexpr Vector subtract(const Vector& other) const
	{
		return Vector(x - other.x, y - other.y, z - other.z);
	}

	constexpr Vector divide(const Vector& other) const
	{
		return Vector(other.x ? x / other.x : 0, other.y ? y / other.y : 0, other.z ? z / other.z : 0);
	}

	constexpr Vector crossProduct(const Vector& other) const
	{
		return Vector(
			(y * other.z) - (z * other.y),
			(z * other.x) - (x * other.z),
			(x * other.y) - (y * other.x)
		);
	}

	constexpr Vector dotProduct(const Vector& other) const
	{
		return Vector(x * other.x, y * other.y, z * other.z);
	}

	constexpr Vector scale(float factor) const
	{
		return factor ? Vector(x / factor, y / factor, z / factor) : Vector(0, 0, 0);
	}

	constexpr Vector invert() const
	{
		return Vector(-x, -y, -z);
	}

	constexpr Vector normalized() const
	{
		return scale(length());
	}

public:
	float x = 0;
	float y = 0;
	float z = 0;

private:
	mutable std::optional<float> m_length;
};

namespace StandardVectors
{
	static inline constexpr auto kOrigin = Vector(0, 0, 0);
	static inline constexpr auto kUnitX = Vector(1, 0, 0);
	static inline constexpr auto kUnitY = Vector(0, 1, 0);
	static inline constexpr auto kUnitZ = Vector(0, 0, 1);
}
