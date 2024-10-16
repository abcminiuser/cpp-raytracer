﻿#include "Engine/Renderer.hpp"

#include "Viewer.hpp"
#include "ExampleScene.hpp"

namespace
{
	constexpr size_t kWidth		= 1920;
	constexpr size_t kHeight	= 1080;
}

int main(int argc, char* argv[])
{
	Renderer renderer(kWidth, kHeight, std::thread::hardware_concurrency());
	Viewer viewer(renderer, kWidth, kHeight);

	auto scene = ExampleScene::Build();
	viewer.view(std::move(scene));
}
