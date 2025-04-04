#pragma once

#include "Engine/Camera.hpp"
#include "Engine/Object.hpp"
#include "Engine/Texture.hpp"

#include <cstdint>
#include <memory>
#include <vector>

struct Scene
{
public:
	std::shared_ptr<Texture>				background;
	Camera									camera;
	std::vector<std::shared_ptr<Object>>	objects;

	uint32_t								samplesPerPixel = 25;
};
