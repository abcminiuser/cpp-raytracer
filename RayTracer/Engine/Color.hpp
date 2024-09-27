#pragma once

#include <algorithm>
#include <stdint.h>

struct Color
{
public:
	constexpr Color(uint8_t r, uint8_t g, uint8_t b)
		: red(r)
		, green(g)
		, blue(b)
	{}

	const uint32_t red = 0;
	const uint32_t green = 0;
	const uint32_t blue = 0;

	constexpr Color multiply(const Color& other) const
	{
		return Color(
			std::min<uint32_t>((red * other.red) / 0xFF, 0xFF),
			std::min<uint32_t>((green * other.green) / 0xFF, 0xFF),
			std::min<uint32_t>((blue * other.blue) / 0xFF, 0xFF)
		);
	}

	constexpr uint32_t toRGBA() const
	{
		return
			0xFFUL << 24 |
			blue << 16 |
			green << 8 |
			red << 0;
	}
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
