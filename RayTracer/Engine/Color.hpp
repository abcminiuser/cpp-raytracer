#pragma once

#include <algorithm>
#include <stdint.h>

struct Color
{
public:
	constexpr			Color()
							: Color(0, 0, 0)
	{

	}

	constexpr			Color(uint8_t r, uint8_t g, uint8_t b)
		: m_red(r)
		, m_green(g)
		, m_blue(b)
	{

	}

	constexpr Color		add(const Color& other) const
	{
		return Color(
			static_cast<uint8_t>(std::min<uint32_t>(m_red + other.m_red, 0xFF)),
			static_cast<uint8_t>(std::min<uint32_t>(m_green + other.m_green, 0xFF)),
			static_cast<uint8_t>(std::min<uint32_t>(m_blue + other.m_blue, 0xFF))
		);
	}

	constexpr Color		multiply(const Color& other) const
	{
		return Color(
			static_cast<uint8_t>(std::min<uint32_t>(m_red * other.m_red, 0xFF)),
			static_cast<uint8_t>(std::min<uint32_t>(m_green * other.m_green, 0xFF)),
			static_cast<uint8_t>(std::min<uint32_t>(m_blue * other.m_blue, 0xFF))
		);
	}

	constexpr Color		scale(float factor) const
	{
		return Color(
			static_cast<uint8_t>(std::min<uint32_t>(static_cast<uint32_t>(m_red * factor), 0xFF)),
			static_cast<uint8_t>(std::min<uint32_t>(static_cast<uint32_t>(m_green * factor), 0xFF)),
			static_cast<uint8_t>(std::min<uint32_t>(static_cast<uint32_t>(m_blue * factor), 0xFF))
		);
	}

	constexpr uint32_t	red() const 	{ return m_red; }
	constexpr uint32_t	green() const 	{ return m_green; }
	constexpr uint32_t	blue() const	{ return m_blue; }

	constexpr uint32_t	toRGBA() const
	{
		return
			0xFFUL << 24 |
			m_blue << 16 |
			m_green << 8 |
			m_red << 0;
	}

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
