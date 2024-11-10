#pragma once

#include <random>

namespace Random
{
	static inline double SignedNormal()
	{
		thread_local std::mt19937 generator;
		thread_local std::uniform_real_distribution<double> distribution(-1.0, 1.0);

		return distribution(generator);
	}

	static inline double UnsignedNormal()
	{
		thread_local std::mt19937 generator;
		thread_local std::uniform_real_distribution<double> distribution(0.0, 1.0);

		return distribution(generator);
	}
}
