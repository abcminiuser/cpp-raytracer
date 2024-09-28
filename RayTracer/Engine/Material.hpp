#pragma once

#include "Color.hpp"

struct Material
{
public:
	Color	color = Color(128, 128, 128);
	double	ambient = 0.1;
	double	diffuse = 0.4;
	double	specular = 0.9;
	double	reflectivity = 0.1;
};
