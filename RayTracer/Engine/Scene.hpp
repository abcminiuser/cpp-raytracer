#pragma once

#include "Engine/Camera.hpp"
#include "Engine/Color.hpp"
#include "Engine/Object.hpp"

#include <memory>
#include <stdint.h>
#include <vector>

struct Scene
{
public:
	Camera									camera;
	Color									background;
	std::vector<std::shared_ptr<Object>>	objects;

	uint8_t									maxRayDepth = 25;
	uint8_t									samplesPerPixel = 25;
	bool									lighting = true;
};
