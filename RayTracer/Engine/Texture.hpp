#pragma once

#include "Engine/Color.hpp"

struct Texture
{
public:
	virtual Color colorAt(double u, double v) const = 0;
};
