#pragma once

#include "Engine/Color.hpp"

struct Texture
{
public:
	virtual			~Texture() = default;

	virtual Color	colorAt(double u, double v) const = 0;
};
