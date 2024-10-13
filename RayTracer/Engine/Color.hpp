#pragma once

#include <algorithm>
#include <stdint.h>
#include <string>
#include <tuple>

struct Color
{
private:
	template <typename T>
	static inline constexpr uint8_t	ClampComponent(T component)
	{
		return static_cast<uint8_t>(std::min<T>(component, 0xFF));
	}

	constexpr auto		tied() const
	{
		return std::tie(m_red, m_green, m_blue);
	}

public:
	constexpr			Color() = default;

	constexpr			Color(uint8_t r, uint8_t g, uint8_t b)
		: m_red(r)
		, m_green(g)
		, m_blue(b)
	{

	}

	constexpr			Color(const Color& other)
		: m_red(other.m_red)
		, m_green(other.m_green)
		, m_blue(other.m_blue)
	{

	}

	constexpr			Color(uint32_t argb32)
		: m_red(static_cast<uint8_t>(argb32 >> 0))
		, m_green(static_cast<uint8_t>(argb32 >> 8))
		, m_blue(static_cast<uint8_t>(argb32 >> 16))
	{

	}

	constexpr Color		add(const Color& other) const
	{
		return Color(
			ClampComponent(m_red + other.m_red),
			ClampComponent(m_green + other.m_green),
			ClampComponent(m_blue + other.m_blue)
		);
	}

	constexpr Color		multiply(const Color& other) const
	{
		return Color(
			ClampComponent(m_red * other.m_red / 255),
			ClampComponent(m_green * other.m_green / 255),
			ClampComponent(m_blue * other.m_blue / 255)
		);
	}

	constexpr Color		scale(double factor) const
	{
		return Color(
			ClampComponent(m_red * factor),
			ClampComponent(m_green * factor),
			ClampComponent(m_blue * factor)
		);
	}

	constexpr uint8_t	red() const 	{ return static_cast<uint8_t>(m_red); }
	constexpr uint8_t	green() const 	{ return static_cast<uint8_t>(m_green); }
	constexpr uint8_t	blue() const	{ return static_cast<uint8_t>(m_blue); }

	constexpr uint32_t	toRGBA() const
	{
		return
			uint32_t(255) << 24 |
			m_blue << 16 |
			m_green << 8 |
			m_red << 0;
	}

	constexpr bool		operator==(const Color& other) const
	{
		return tied() == other.tied();
	}

	std::string			string() const;

private:
	uint32_t			m_red = 0;
	uint32_t			m_green = 0;
	uint32_t			m_blue = 0;
};

namespace Palette
{
	static inline constexpr auto kRed = Color(255, 0, 0);
	static inline constexpr auto kGreen = Color(0, 255, 0);
	static inline constexpr auto kBlue = Color(0, 0, 255);
	static inline constexpr auto kBlack = Color(0, 0, 0);
	static inline constexpr auto kWhite = Color(255, 255, 255);
	static inline constexpr auto kCyan = Color(0, 255, 255);
	static inline constexpr auto kYellow = Color(255, 255, 0);
	static inline constexpr auto kMagenta = Color(255, 0, 255);
}
