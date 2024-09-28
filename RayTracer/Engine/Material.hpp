#pragma once

#include "Texture.hpp"

#include <memory>

struct Material
{
public:
	std::unique_ptr<Texture>	texture;
	double						ambient = 0.1;
	double						diffuse = 0.4;
	double						specular = 0.9;
	double						reflectivity = 0.1;
};
