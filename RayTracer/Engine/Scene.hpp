#pragma once

#include "Camera.hpp"
#include "Color.hpp"
#include "Light.hpp"
#include "Object.hpp"

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
