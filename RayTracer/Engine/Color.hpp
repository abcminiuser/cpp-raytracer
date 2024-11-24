#pragma once

#include <algorithm>
#include <cstdint>
#include <string>

struct Color
{
public:
	static constexpr Color	FromRGB888(uint8_t r, uint8_t g, uint8_t b)
	{
		return Color(r / 255.0, g / 255.0, b / 255.0);
	}

	static constexpr Color	FromRGBA8888(uint32_t rgba32)
	{
		uint8_t r	= static_cast<uint8_t>(rgba32 >> 0);
		uint8_t g	= static_cast<uint8_t>(rgba32 >> 8);
		uint8_t b	= static_cast<uint8_t>(rgba32 >> 16);

		return FromRGB888(r, g, b);
	}

	constexpr				Color() = default;

	constexpr				Color(double r, double g, double b)
		: m_red(r)
		, m_green(g)
		, m_blue(b)
	{

	}

	constexpr bool			operator==(const Color& other) const = default;

	constexpr Color&		operator+=(const Color& other)
	{
		m_red += other.m_red;
		m_green += other.m_green;
		m_blue += other.m_blue;

		return *this;
	}

	constexpr Color&		operator-=(const Color& other)
	{
		m_red -= other.m_red;
		m_green -= other.m_green;
		m_blue -= other.m_blue;

		return *this;
	}

	constexpr Color&		operator*=(const Color& other)
	{
		m_red *= other.m_red;
		m_green *= other.m_green;
		m_blue *= other.m_blue;

		return *this;
	}

	constexpr Color&		operator*=(double factor)
	{
		m_red *= factor;
		m_green *= factor;
		m_blue *= factor;

		return *this;
	}

	constexpr Color&		operator/=(const Color& other)
	{
		m_red /= other.m_red;
		m_green /= other.m_green;
		m_blue /= other.m_blue;

		return *this;
	}

	constexpr Color&		operator/=(double factor)
	{
		m_red /= factor;
		m_green /= factor;
		m_blue /= factor;

		return *this;
	}

	constexpr Color			operator+(const Color& other) const
	{
		return Color(
			m_red + other.m_red,
			m_green + other.m_green,
			m_blue + other.m_blue
		);
	}

	constexpr Color			operator-(const Color& other) const
	{
		return Color(
			m_red - other.m_red,
			m_green - other.m_green,
			m_blue - other.m_blue
		);
	}

	constexpr Color			operator*(const Color& other) const
	{
		return Color(
			m_red * other.m_red,
			m_green * other.m_green,
			m_blue * other.m_blue
		);
	}

	constexpr Color			operator*(double factor) const
	{
		return Color(
			m_red * factor,
			m_green * factor,
			m_blue * factor
		);
	}

	constexpr Color			operator/(const Color& other) const
	{
		return Color(
			m_red / other.m_red,
			m_green / other.m_green,
			m_blue / other.m_blue
		);
	}

	constexpr Color			operator/(double factor) const
	{
		return Color(
			m_red / factor,
			m_green / factor,
			m_blue / factor
		);
	}

	constexpr double		average() const
	{
		return (m_red + m_green + m_blue) / 3;
	}

	constexpr Color			clamped() const
	{
		return Color(
			std::clamp(m_red, 0.0, 1.0),
			std::clamp(m_green, 0.0, 1.0),
			std::clamp(m_blue, 0.0, 1.0)
		);
	}

	constexpr double		red() const 	{ return m_red; }
	constexpr double		green() const 	{ return m_green; }
	constexpr double		blue() const	{ return m_blue; }

	constexpr uint32_t		toRGBA8888() const
	{
		return
			(static_cast<uint32_t>(255) & 0xFF) << 24 |
			(static_cast<uint32_t>(m_blue * 255.0) & 0xFF) << 16 |
			(static_cast<uint32_t>(m_green * 255.0) & 0xFF) << 8 |
			(static_cast<uint32_t>(m_red * 255.0) & 0xFF) << 0;
	}

	std::string				string() const;

private:
	double					m_red = 0;
	double					m_green = 0;
	double					m_blue = 0;
};

namespace Palette
{
	static inline constexpr auto kRed = Color::FromRGB888(255, 0, 0);
	static inline constexpr auto kGreen = Color::FromRGB888(0, 255, 0);
	static inline constexpr auto kBlue = Color::FromRGB888(0, 0, 255);
	static inline constexpr auto kBlack = Color::FromRGB888(0, 0, 0);
	static inline constexpr auto kWhite = Color::FromRGB888(255, 255, 255);
	static inline constexpr auto kCyan = Color::FromRGB888(0, 255, 255);
	static inline constexpr auto kYellow = Color::FromRGB888(255, 255, 0);
	static inline constexpr auto kMagenta = Color::FromRGB888(255, 0, 255);
}
