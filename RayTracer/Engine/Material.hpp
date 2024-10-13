#pragma once

struct Material
{
public:
	double	ambient = 0.1;
	double	diffuse = 0.3;
	double	specular = 0.5;
	double	reflectivity = 0.2;
	double	refractivity = 0.0;
	double	refractionIndex = 1.0;
};
