#pragma once

#include <numbers>

namespace MathUtil
{
	constexpr double DegreesToRadians(double degrees)
	{
		return degrees * (std::numbers::pi / 180);
	}
}
