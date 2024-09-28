#include "ExampleScene.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Object/PlaneObject.hpp"
#include "Engine/Object/SphereObject.hpp"
#include "Engine/Texture/SolidTexture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"
#include "Engine/Texture/ImageTexture.hpp"

#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <stdint.h>

namespace
{
	std::shared_ptr<ImageTexture> MakeImageTexture(const std::string& path)
	{
		sf::Image imageTexture;
		if (! imageTexture.loadFromFile(path))
			throw std::runtime_error("Failed to load image: " + path);

		const auto	dimensions	= imageTexture.getSize();
		const auto* pixels		= imageTexture.getPixelsPtr();

		return std::make_shared<ImageTexture>(dimensions.x, dimensions.y, reinterpret_cast<const uint32_t*>(pixels));
	}
}

Scene ExampleScene::Build()
{
	Scene scene;

	scene.background = Palette::kWhite.scale(0.1);

	scene.camera = Camera(
		/* Position: */ 				Vector(0, 2, -8),
		/* Target: */					StandardVectors::kUnitZ,
		/* View Width: */				4.0,
		/* View Height: */				9.0 / 4.0
	);

	scene.lights.push_back(
		std::make_unique<Light>(
		/* Position: */					Vector(5, 10, -5),
		/* Color */   					Palette::kWhite
		)
	);

	scene.lights.push_back(
		std::make_unique<Light>(
		/* Position: */					Vector(-5, 3, -5),
		/* Color */   					Palette::kRed
		)
	);

	scene.objects.push_back(
		std::make_unique<PlaneObject>(
		/* Normal: */					StandardVectors::kUnitY,
		/* Distance to Normal */		-1,
		/* Material: */					Material{
				.texture = MakeImageTexture("Assets/Test.png")
			},
		/* Texture Scaling: */			1 / 3.0
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
		/* Position: */					Vector(-4, 0, 4),
		/* Material: */					Material{
				.texture = std::make_shared<SolidTexture>(Palette::kWhite)
			},
		/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
		/* Position: */					Vector(-2, 0, 2),
		/* Material: */					Material{
				.texture = std::make_shared<CheckerboardTexture>(Palette::kMagenta, Palette::kYellow, 8)
			},
		/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
		/* Position: */					Vector( 0, 0, 0),
		/* Material: */					Material{
				.texture = std::make_shared<SolidTexture>(Palette::kRed)
			},
		/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
		/* Position: */					Vector( 2, 0, 2),
		/* Material: */					Material{
				.texture = std::make_shared<SolidTexture>(Palette::kGreen)
			},
		/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
		/* Position: */					Vector( 4, 0, 4),
		/* Material: */					Material{
				.texture = std::make_shared<SolidTexture>(Palette::kBlue)
			},
		/* Radius: */					1
		)
	);

	return scene;
}
