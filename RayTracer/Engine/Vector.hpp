#pragma once

#include "Engine/Random.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

struct Vector
{
public:
	constexpr			Vector() = default;

	constexpr			Vector(double x, double y, double z)
		: m_x(x)
		, m_y(y)
		, m_z(z)
	{

	}

	constexpr bool		operator==(const Vector& other) const = default;

	constexpr Vector&	operator+=(const Vector& other)
	{
		m_x += other.m_x;
		m_y += other.m_y;
		m_z += other.m_z;

		return *this;
	}

	constexpr Vector&	operator-=(const Vector& other)
	{
		m_x -= other.m_x;
		m_y -= other.m_y;
		m_z -= other.m_z;

		return *this;
	}

	constexpr Vector&	operator*=(const Vector& other)
	{
		m_x *= other.m_x;
		m_y *= other.m_y;
		m_z *= other.m_z;

		return *this;
	}

	constexpr Vector&	operator*=(double factor)
	{
		m_x *= factor;
		m_y *= factor;
		m_z *= factor;

		return *this;
	}

	constexpr Vector&	operator/=(const Vector& other)
	{
		m_x /= other.m_x;
		m_y /= other.m_y;
		m_z /= other.m_z;

		return *this;
	}

	constexpr Vector&	operator/=(double factor)
	{
		m_x /= factor;
		m_y /= factor;
		m_z /= factor;

		return *this;
	}

	constexpr Vector	operator+(const Vector& other) const
	{
		return Vector(
			m_x + other.m_x,
			m_y + other.m_y,
			m_z + other.m_z
		);
	}

	constexpr Vector	operator-(const Vector& other) const
	{
		return Vector(
			m_x - other.m_x,
			m_y - other.m_y,
			m_z - other.m_z
		);
	}

	constexpr Vector	operator*(const Vector& other) const
	{
		return Vector(
			m_x * other.m_x,
			m_y * other.m_y,
			m_z * other.m_z
		);
	}

	constexpr Vector	operator*(double factor) const
	{
		return Vector(
			m_x * factor,
			m_y * factor,
			m_z * factor
		);
	}

	constexpr Vector	operator/(const Vector& other) const
	{
		return Vector(
			m_x / other.m_x,
			m_y / other.m_y,
			m_z / other.m_z
		);
	}

	constexpr Vector	operator/(double factor) const
	{
		return Vector(
			m_x / factor,
			m_y / factor,
			m_z / factor
		);
	}

	constexpr Vector	crossProduct(const Vector& other) const
	{
		return Vector(
			(m_y * other.m_z) - (m_z * other.m_y),
			(m_z * other.m_x) - (m_x * other.m_z),
			(m_x * other.m_y) - (m_y * other.m_x)
		);
	}

	constexpr double	dotProduct(const Vector& other) const
	{
		return (m_x * other.m_x) + (m_y * other.m_y) + (m_z * other.m_z);
	}

	constexpr Vector	inverted() const
	{
		return Vector(-m_x, -m_y, -m_z);
	}

	constexpr double	lengthSquared() const
	{
		return (m_x * m_x) + (m_y * m_y) + (m_z * m_z);
	}

	double				length() const
	{
		return std::sqrt(lengthSquared());
	}

	Vector				unit() const
	{
		return *this / length();
	}

	bool				isUnit() const
	{
		return (length() - 1) <= std::numeric_limits<double>::epsilon();
	}

	constexpr double	x() const	{ return m_x; }
	constexpr double	y() const	{ return m_y; }
	constexpr double	z() const	{ return m_z; }

	std::string			string() const;

private:
	double				m_x = 0;
	double				m_y = 0;
	double				m_z = 0;
};

namespace VectorUtils
{
	static inline constexpr Vector MinPoint(const Vector& v1, const Vector& v2)
	{
		return Vector(
			std::min(v1.x(), v2.x()),
			std::min(v1.y(), v2.y()),
			std::min(v1.z(), v2.z())
		);
	}

	static inline constexpr Vector MaxPoint(const Vector& v1, const Vector& v2)
	{
		return Vector(
			std::max(v1.x(), v2.x()),
			std::max(v1.y(), v2.y()),
			std::max(v1.z(), v2.z())
		);
	}

	static inline constexpr double MinComponent(const Vector& v)
	{
		return std::min(std::min(v.x(), v.y()), v.z());
	}

	static inline constexpr double MaxComponent(const Vector& v)
	{
		return std::max(std::max(v.x(), v.y()), v.z());
	}

	static inline Vector RandomUnitVector()
	{
	    for (;;)
	    {
			auto v = Vector(Random::SignedNormal(), Random::SignedNormal(), Random::SignedNormal());
			if (v.lengthSquared() > 1e-10 && v.lengthSquared() <= 1)
				return v.unit();
		}
	}
}

namespace StandardVectors
{
	static inline constexpr auto kZero = Vector(0, 0, 0);
	static inline constexpr auto kOrigin = Vector(0, 0, 0);
	static inline constexpr auto kUnitX = Vector(1, 0, 0);
	static inline constexpr auto kUnitY = Vector(0, 1, 0);
	static inline constexpr auto kUnitZ = Vector(0, 0, 1);
	static inline constexpr auto kUnit = Vector(1, 1, 1);

	static inline constexpr auto kMin = Vector(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());
	static inline constexpr auto kMax = Vector(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
}
