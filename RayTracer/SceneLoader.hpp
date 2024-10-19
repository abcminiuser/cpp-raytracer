#pragma once

#include "Engine/Scene.hpp"

#include <filesystem>

namespace SceneLoader
{
	Scene	Load(const std::filesystem::path& path);
}
