#include "ExampleScene.hpp"

#include "Engine/Mesh.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Material/DebugMaterial.hpp"
#include "Engine/Material/DielectricMaterial.hpp"
#include "Engine/Material/DiffuseMaterial.hpp"
#include "Engine/Material/LightMaterial.hpp"
#include "Engine/Material/ReflectiveMaterial.hpp"
#include "Engine/Object/BoxObject.hpp"
#include "Engine/Object/PlaneObject.hpp"
#include "Engine/Object/MeshObject.hpp"
#include "Engine/Object/SphereObject.hpp"
#include "Engine/Texture/SolidTexture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"
#include "Engine/Texture/ImageTexture.hpp"

#include <SFML/Graphics.hpp>

#include <OBJ_Loader.h>

#include <numbers>
#include <stdexcept>
#include <stdint.h>
#include <vector>

namespace
{
	std::shared_ptr<ImageTexture> MakeImageTexture(const std::string& path, Color multiplier = Color(1, 1, 1))
	{
		sf::Image imageTexture;
		if (! imageTexture.loadFromFile(path))
			throw std::runtime_error("Failed to load image: " + path);

		const auto	dimensions	= imageTexture.getSize();
		const auto* pixels		= imageTexture.getPixelsPtr();

		return std::make_shared<ImageTexture>(dimensions.x, dimensions.y, reinterpret_cast<const uint32_t*>(pixels), multiplier);
	}

	std::shared_ptr<Mesh> MakeObjectMesh(const std::string& path, double scale = 1.0)
	{
		objl::Loader objLoader;
		if (! objLoader.LoadFile(path))
			throw std::runtime_error("Failed to load object file: " + path);

		std::vector<Vertex>		vertices;
		std::vector<Triangle>	triangles;

		for (const auto& mesh : objLoader.LoadedMeshes)
		{
			printf("Adding sub-mesh '%s'\n", mesh.MeshName.empty() ? "(Unnamed)" : mesh.MeshName.c_str());

			const uint32_t meshStartPos = static_cast<uint32_t>(vertices.size());

			vertices.reserve(vertices.size() + mesh.Vertices.size());
			triangles.reserve(triangles.size() + (mesh.Indices.size() / 3));

			for (const auto& v : mesh.Vertices)
			{
				vertices.emplace_back(
					Vertex
					{
						.position = Vector(double(v.Position.X), double(v.Position.Y), double(v.Position.Z)).scale(scale),
						.normal = Vector(double(v.Normal.X), double(v.Normal.Y), double(v.Normal.Z)),
						.texture = Vector(double(v.TextureCoordinate.X), double(v.TextureCoordinate.Y), 0.0)
					}
				);
			}

			for (size_t i = 0; i < mesh.Indices.size(); i += 3)
				triangles.emplace_back(Triangle{ meshStartPos + mesh.Indices[i + 0], meshStartPos + mesh.Indices[i + 1], meshStartPos + mesh.Indices[i + 2] });
		}

		return std::make_shared<Mesh>(std::move(vertices), std::move(triangles));
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

	scene.objects.push_back(
		std::make_unique<BoxObject>(
			/* Position: */					Vector(-1e20, -1e20, -50),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<LightMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kWhite.scale(.85))
			),
			/* Size: */						Vector(2e20, 2e20, 1)
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-6, 3, -5),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<LightMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Color::FromComponentRGB(255, 128, 0).scale(1.4))
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<PlaneObject>(
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				MakeImageTexture("Assets/Brick.jpg")
			),
			/* Normal: */					StandardVectors::kUnitY,
			/* Distance to Normal: */		-1,
			/* Texture Scaling: */			1 / 3.0
		)
	);

	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-4, 0, 4),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kWhite)
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-2, 0, 2),
			/* Rotation: */					Vector(0, std::numbers::pi / 8, std::numbers::pi / 8),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				std::make_shared<CheckerboardTexture>(Palette::kMagenta, Palette::kYellow, 8)
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector( 0, 0, 0),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kRed)
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector( 2, 0, 2),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kGreen)
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector( 4, 0, 4),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kBlue)
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(6, 0, 0),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kWhite)
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-6, 0, 0),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				MakeImageTexture("Assets/Marble.jpg")
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<SphereObject>(
			/* Position: */					Vector(-3, 0, -3),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<DielectricMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kWhite),
				/* Refraction Index: */		2.2
			),
			/* Radius: */					1
		)
	);
	scene.objects.push_back(
		std::make_unique<BoxObject>(
			/* Position: */					Vector(0, 0, 6),
			/* Rotation: */					Vector(0, 0, 0),
			/* Material: */					std::make_shared<ReflectiveMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kWhite),
				/* Polish: */				1.0
			),
			/* Size: */						Vector(2, 2, 2)
		)
	);
	scene.objects.push_back(
		std::make_unique<BoxObject>(
			/* Position: */					Vector(1, 0, -4),
			/* Rotation: */					Vector(0, std::numbers::pi / 4, 0),
			/* Material: */					std::make_shared<DiffuseMaterial>(
				/* Texture: */				MakeImageTexture("Assets/Cube.png")
			),
			/* Size: */						Vector(1, 1, 1)
		)
	);
	scene.objects.push_back(
		std::make_unique<MeshObject>(
			/* Position: */					Vector(8, 0, 8),
			/* Rotation: */					Vector(0, std::numbers::pi / 4, 0),
			/* Material: */					std::make_shared<ReflectiveMaterial>(
				/* Texture: */				std::make_shared<SolidTexture>(Palette::kWhite),
				/* Polish: */				0.99
			),
			/* Mesh: */						MakeObjectMesh("Assets/Teapot.obj", 1.0)
		)
	);

	return scene;
}
