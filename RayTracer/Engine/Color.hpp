#pragma once

#include <algorithm>
#include <stdint.h>
#include <string>
#include <tuple>

struct Color
{
private:
	constexpr auto			tied() const
	{
		return std::tie(m_red, m_green, m_blue);
	}

public:
	static constexpr Color	FromComponentRGB(uint8_t r, uint8_t g, uint8_t b)
	{
		return Color(r / 255.0, g / 255.0, b / 255.0);
	}

	static constexpr Color	FromPackedRGBA(uint32_t rgba32)
	{
		uint8_t red		= static_cast<uint8_t>(rgba32 >> 0);
		uint8_t green	= static_cast<uint8_t>(rgba32 >> 8);
		uint8_t blue	= static_cast<uint8_t>(rgba32 >> 16);

		return FromComponentRGB(red, green, blue);
	}

	constexpr				Color() = default;

	constexpr				Color(double r, double g, double b)
		: m_red(r)
		, m_green(g)
		, m_blue(b)
	{

	}

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

	constexpr double		red() const 	{ return m_red; }
	constexpr double		green() const 	{ return m_green; }
	constexpr double		blue() const	{ return m_blue; }

	constexpr uint32_t		toPackedRGBA() const
	{
		return
			static_cast<uint32_t>(255) << 24 |
			static_cast<uint32_t>(std::clamp(m_blue, 0.0, 1.0) * 255.0) << 16 |
			static_cast<uint32_t>(std::clamp(m_green, 0.0, 1.0) * 255.0) << 8 |
			static_cast<uint32_t>(std::clamp(m_red, 0.0, 1.0) * 255.0) << 0;
	}

	constexpr bool			operator==(const Color& other) const
	{
		return tied() == other.tied();
	}

	std::string				string() const;

private:
	double					m_red = 0;
	double					m_green = 0;
	double					m_blue = 0;
};

namespace Palette
{
	static inline constexpr auto kRed = Color::FromComponentRGB(255, 0, 0);
	static inline constexpr auto kGreen = Color::FromComponentRGB(0, 255, 0);
	static inline constexpr auto kBlue = Color::FromComponentRGB(0, 0, 255);
	static inline constexpr auto kBlack = Color::FromComponentRGB(0, 0, 0);
	static inline constexpr auto kWhite = Color::FromComponentRGB(255, 255, 255);
	static inline constexpr auto kCyan = Color::FromComponentRGB(0, 255, 255);
	static inline constexpr auto kYellow = Color::FromComponentRGB(255, 255, 0);
	static inline constexpr auto kMagenta = Color::FromComponentRGB(255, 0, 255);
}
