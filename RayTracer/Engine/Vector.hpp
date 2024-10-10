#pragma once

#include <algorithm>
#include <cmath>
#include <tuple>

struct Vector
{
private:
	constexpr auto		tied() const
	{
		return std::tie(m_x, m_y, m_z);
	}

public:
	constexpr			Vector() = default;

	constexpr			Vector(double x, double y, double z)
		: m_x(x)
		, m_y(y)
		, m_z(z)
	{

	}

	constexpr			Vector(const Vector& other)
		: m_x(other.m_x)
		, m_y(other.m_y)
		, m_z(other.m_z)
	{

	}

	constexpr Vector&	operator=(const Vector& other) = default;

	constexpr double	squaredEuclidianDistance() const
	{
		if (! m_squaredEuclidianDistance)
			m_squaredEuclidianDistance = (m_x * m_x) + (m_y * m_y) + (m_z * m_z);

		return m_squaredEuclidianDistance;
	}

	constexpr double	length() const
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

	constexpr Vector	divide(const Vector& other) const
	{
		return Vector(m_x / other.m_x, m_y / other.m_y, m_z / other.m_z);
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

	constexpr Vector	scale(double factor) const
	{
		return Vector(m_x * factor, m_y * factor, m_z * factor);
	}

	constexpr Vector	invert() const
	{
		return Vector(-m_x, -m_y, -m_z);
	}

	constexpr Vector	unit() const
	{
		return scale(1 / length());
	}

	constexpr double	x() const	{ return m_x; }
	constexpr double	y() const	{ return m_y; }
	constexpr double	z() const	{ return m_z; }

	constexpr bool		operator==(const Vector& other) const
	{
		return tied() == other.tied();
	}

	static constexpr Vector MinPoint(const Vector& v1, const Vector& v2)
	{
		return Vector(
			std::min(v1.x(), v2.x()),
			std::min(v1.y(), v2.y()),
			std::min(v1.z(), v2.z())
		);
	}

	static constexpr Vector MaxPoint(const Vector& v1, const Vector& v2)
	{
		return Vector(
			std::max(v1.x(), v2.x()),
			std::max(v1.y(), v2.y()),
			std::max(v1.z(), v2.z())
		);
	}

private:
	double				m_x = 0;
	double				m_y = 0;
	double				m_z = 0;

private:
	mutable double		m_squaredEuclidianDistance = 0;
	mutable double		m_length = 0;
};

namespace StandardVectors
{
	static inline constexpr auto kOrigin = Vector(0, 0, 0);
	static inline constexpr auto kUnitX = Vector(1, 0, 0);
	static inline constexpr auto kUnitY = Vector(0, 1, 0);
	static inline constexpr auto kUnitZ = Vector(0, 0, 1);

	static inline constexpr auto kMin = Vector(std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min());
	static inline constexpr auto kMax = Vector(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
}
