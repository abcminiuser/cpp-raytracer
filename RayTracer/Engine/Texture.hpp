#pragma once

#include "Engine/Color.hpp"

class Texture
{
public:
	virtual			~Texture() = default;

	Color			sample(double u, double v) const;

protected:
	virtual Color	colorAt(double u, double v) const = 0;
};
