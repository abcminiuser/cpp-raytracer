#pragma once

#include "Color.hpp"

struct Material
{
public:
	Color	color = Color(128, 128, 128);
	float	ambient = 0.1f;
	float	reflectivity = 0.2f;
};
