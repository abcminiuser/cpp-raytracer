#pragma once

#include "Engine/Camera.hpp"
#include "Engine/Color.hpp"
#include "Engine/Light.hpp"
#include "Engine/Object.hpp"

#include <memory>
#include <stdint.h>
#include <vector>

struct Scene
{
public:
	Camera									camera;
	Color									background;
	std::vector<std::shared_ptr<Light>>		lights;
	std::vector<std::shared_ptr<Object>>	objects;
	double									airRefractionIndex = 1.0;
	uint8_t									maxRayDepth = 10;
};
