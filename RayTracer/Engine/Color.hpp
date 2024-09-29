#pragma once

#include <algorithm>
#include <stdint.h>

struct Color
{
private:
	template <typename T>
	static inline constexpr uint8_t	ClampComponent(T component)
	{
		return (component < 0xFF) ? static_cast<uint8_t>(component) : 0xFF;
	}

public:
	constexpr			Color()
							: Color(0, 0, 0)
	{

	}

	constexpr			Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: m_red(r)
		, m_green(g)
		, m_blue(b)
		, m_alpha(a)
	{

	}

	constexpr			Color(uint32_t argb32)
		: m_red(static_cast<uint8_t>(argb32 >> 0))
		, m_green(static_cast<uint8_t>(argb32 >> 8))
		, m_blue(static_cast<uint8_t>(argb32 >> 16))
		, m_alpha(static_cast<uint8_t>(argb32 >> 24))
	{

	}

	constexpr Color		add(const Color& other) const
	{
		return Color(
			ClampComponent(m_red + other.m_red),
			ClampComponent(m_green + other.m_green),
			ClampComponent(m_blue + other.m_blue),
			ClampComponent(m_alpha + other.m_alpha)
		);
	}

	constexpr Color		multiply(const Color& other) const
	{
		return Color(
			ClampComponent(m_red * other.m_red),
			ClampComponent(m_green * other.m_green),
			ClampComponent(m_blue * other.m_blue),
			ClampComponent(m_alpha * other.m_alpha)
		);
	}

	constexpr Color		scale(double factor) const
	{
		return Color(
			ClampComponent(m_red * factor),
			ClampComponent(m_green * factor),
			ClampComponent(m_blue * factor),
			ClampComponent(m_alpha * factor)
		);
	}

	constexpr uint32_t	red() const 	{ return m_red; }
	constexpr uint32_t	green() const 	{ return m_green; }
	constexpr uint32_t	blue() const	{ return m_blue; }
	constexpr uint32_t	alpha() const	{ return m_alpha; }

	constexpr uint32_t	toRGBA() const
	{
		return
			m_alpha << 24 |
			m_blue << 16 |
			m_green << 8 |
			m_red << 0;
	}

private:
	uint32_t			m_red = 0;
	uint32_t			m_green = 0;
	uint32_t			m_blue = 0;
	uint32_t			m_alpha = 0;
};

namespace Palette
{
	static inline constexpr auto kTransparent = Color(0, 0, 0, 0);
	static inline constexpr auto kRed = Color(255, 0, 0);
	static inline constexpr auto kGreen = Color(0, 255, 0);
	static inline constexpr auto kBlue = Color(0, 0, 255);
	static inline constexpr auto kBlack = Color(0, 0, 0);
	static inline constexpr auto kWhite = Color(255, 255, 255);
	static inline constexpr auto kCyan = Color(0, 255, 255);
	static inline constexpr auto kYellow = Color(255, 255, 0);
	static inline constexpr auto kMagenta = Color(255, 0, 255);
}
