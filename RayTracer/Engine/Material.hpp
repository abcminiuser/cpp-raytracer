#pragma once

struct Material
{
public:
	double	ambient = 0.1;
	double	diffuse = 0.4;
	double	specular = 0.9;
	double	reflectivity = 0.1;
	double	refractivity = 0.1;
	double	refractionIndex = 1.0;
};
