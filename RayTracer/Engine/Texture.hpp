#pragma once

#include "Engine/Color.hpp"

class Texture
{
public:
	virtual			~Texture() = default;

	virtual Color	colorAt(double u, double v) const = 0;
};
