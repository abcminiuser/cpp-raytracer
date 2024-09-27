#pragma once

#include "Color.hpp"
#include "Camera.hpp"
#include "Object.hpp"

#include <memory>
#include <vector>

struct Scene
{
public:
	Camera									camera;
	Color									background;
	std::vector<std::shared_ptr<Object>>	objects;
};
