#pragma once

#include "Engine/Camera.hpp"
#include "Engine/Color.hpp"
#include "Engine/Light.hpp"
#include "Engine/Object.hpp"

#include <memory>
#include <vector>

struct Scene
{
public:
	Camera									camera;
	Color									background;
	std::vector<std::unique_ptr<Light>>		lights;
	std::vector<std::unique_ptr<Object>>	objects;
};
