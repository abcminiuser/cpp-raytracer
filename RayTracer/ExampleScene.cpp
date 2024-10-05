#include "ExampleScene.hpp"

#include "Engine/Mesh.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Object/BoxObject.hpp"
#include "Engine/Object/PlaneObject.hpp"
#include "Engine/Object/MeshObject.hpp"
#include "Engine/Object/SphereObject.hpp"
#include "Engine/Texture/SolidTexture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"
#include "Engine/Texture/ImageTexture.hpp"

#include <SFML/Graphics.hpp>

#include <OBJ_Loader.h>

#include <stdexcept>
#include <stdint.h>
#include <vector>

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

	std::shared_ptr<Mesh> MakeObjectMesh(const std::string& path)
	{
		objl::Loader l;
		l.LoadFile(path);

		std::vector<Vertex>		vertices;
		std::vector<Triangle>	triangles;

		for (const auto& v : l.LoadedVertices)
		{
			vertices.push_back(
				Vertex
				{
					.position = Vector(v.Position.X, v.Position.Y, v.Position.Z),
					.normal = Vector(v.Normal.X, v.Normal.Y, v.Normal.Z).unit(),
					.texture = Vector(v.TextureCoordinate.X, v.TextureCoordinate.Y, 0)
				}
			);
		}

		for (size_t i = 0; i < l.LoadedIndices.size(); i += 3)
			triangles.push_back(Triangle{ l.LoadedIndices[i + 0], l.LoadedIndices[i + 1], l.LoadedIndices[i + 2]});

		return BuildMesh(vertices, triangles);
	}
}

Scene ExampleScene::Build()
{
	Scene scene;

	scene.background = Palette::kWhite.scale(0.1);

	scene.camera = Camera(
		/* Position: */ 					Vector(0, 4, -8),
		/* Target: */						Vector(0, 0, 0),
		/* View Width: */					9.0 / 2,
		/* View Height: */					5.0 / 2
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
			/* Color */   					Color(255, 128, 0)
		)
	);

	scene.objects.push_back(
		std::make_unique<PlaneObject>(
			/* Normal: */					StandardVectors::kUnitY,
			/* Distance to Normal: */		-1,
			/* Texture: */					MakeImageTexture("Assets/Brick.jpg"),
			/* Texture Scaling: */			1 / 3.0,
			/* Material: */					Material{
				.ambient = 0.2,
				.diffuse = 0.1,
				.specular = 0.0,
				.reflectivity = 0.0
			}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-4, 0, 4),
			/* Radius: */					1,
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kWhite),
			/* Material: */					Material{}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-2, 0, 2),
			/* Radius: */					1,
			/* Texture */					std::make_shared<CheckerboardTexture>(Palette::kMagenta, Palette::kYellow, 8),
			/* Material: */					Material{}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector( 0, 0, 0),
			/* Radius: */					1,
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kRed),
			/* Material: */					Material{}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector( 2, 0, 2),
			/* Radius: */					1,
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kGreen),
			/* Material: */					Material{}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector( 4, 0, 4),
			/* Radius: */					1,
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kBlue),
			/* Material: */					Material{}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(6, 0, 0),
			/* Radius: */					1,
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kWhite),
			/* Material: */					Material{
				.ambient = 0.1,
				.diffuse = 0.0,
				.specular = 0.5,
				.reflectivity = 0.7
			}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-6, 0, 0),
			/* Radius: */					1,
			/* Texture: */					MakeImageTexture("Assets/Marble.jpg"),
			/* Material: */					Material{
				.ambient = 0.3,
				.diffuse = 0.15,
				.specular = 0.0,
				.reflectivity = 0.0
			}
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-3, 0, -3),
			/* Radius: */					1,
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kTransparent),
			/* Material: */					Material{
				.ambient = 0.0,
				.diffuse = 0.0,
				.specular = 0.1,
				.reflectivity = 0.0,
				.refractivity = 0.9,
				.refractionIndex = 1.5,
			}
		)
	);
	scene.objects.push_back(
		std::make_unique<BoxObject>(
			/* Position: */					Vector(0, 0, 6),
			/* Size: */						Vector(2, 2, 2),
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kWhite),
			/* Material: */					Material{
				.ambient = 0.1,
				.diffuse = 0.0,
				.specular = 0.5,
				.reflectivity = 0.7
			}
		)
	);
	scene.objects.push_back(
		std::make_unique<BoxObject>(
			/* Position: */					Vector(1, 0, -4),
			/* Size: */						Vector(1, 1, 1),
			/* Texture: */					MakeImageTexture("Assets/Cube.png"),
			/* Material: */					Material{
				.ambient = 0.25,
				.diffuse = 0.0,
				.specular = 0.5,
				.reflectivity = 0.0
			}
		)
	);
	scene.objects.push_back(
		std::make_unique<MeshObject>(
			/* Position: */					Vector(8, 0, 8),
			/* Mesh: */						MakeObjectMesh("Assets/Teapot.obj"),
			/* Texture: */					std::make_shared<SolidTexture>(Palette::kWhite),
			/* Material: */					Material{
				.ambient = 0.2,
				.diffuse = 0.2,
				.specular = 0.3,
				.reflectivity = 0.0
			}
		)
	);

	return scene;
}
