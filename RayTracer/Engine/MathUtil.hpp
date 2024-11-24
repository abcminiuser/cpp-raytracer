#pragma once

#include "Engine/Vector.hpp"

#include <cmath>
#include <numbers>

namespace MathUtil
{
	constexpr double DegreesToRadians(double degrees)
	{
		return degrees * (std::numbers::pi / 180);
	}

	constexpr double RadiansToDegrees(double radians)
	{
		return radians * (180 / std::numbers::pi);
	}

	constexpr Vector CartesianToPolar(const Vector& vector)
	{
		return Vector(
			std::atan2(vector.z(), vector.x()) / (2 * std::numbers::pi),
			std::asin(vector.y()) / std::numbers::pi,
			0);
	}
}
