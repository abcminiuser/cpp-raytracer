#pragma once

#include <stdint.h>

struct Color
{
public:
	constexpr Color(uint8_t r, uint8_t g, uint8_t b)
		: red(r)
		, green(g)
		, blue(b)
	{}

	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	constexpr uint32_t toRGBA() const
	{
		return
			static_cast<uint32_t>(0xFF) << 24 |
			static_cast<uint32_t>(blue) << 16 |
			static_cast<uint32_t>(green) << 8 |
			static_cast<uint32_t>(red) << 0;
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
