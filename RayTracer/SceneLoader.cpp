#include "SceneLoader.hpp"

#include "Engine/Color.hpp"
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

#include <fkYAML/node.hpp>

#include <fstream>
#include <map>
#include <numbers>
#include <regex>
#include <stdexcept>
#include <stdint.h>
#include <vector>

namespace
{
	std::string TrimWhitespace(std::string value)
	{
		if (auto pos = value.find_first_not_of(" \t"); pos != std::string::npos)
			value = value.substr(pos);

		if (auto pos = value.find_last_not_of(" \t"); pos != std::string::npos)
			value = value.substr(0, pos + 1);

		return value;
	}

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

	Color ParseColor(std::string value)
	{
		value = TrimWhitespace(value);

		static const std::regex normColorRegex("Color\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
		if (std::smatch matches; std::regex_search(value, matches, normColorRegex))
		{
			const auto r = std::stod(matches[1]);
			const auto g = std::stod(matches[2]);
			const auto b = std::stod(matches[3]);

			return Color(r, g, b);
		}

		static const std::regex componentColorRegex("ComponentColor\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
		if (std::smatch matches; std::regex_search(value, matches, componentColorRegex))
		{
			const auto r = std::clamp(std::stoi(matches[1]), 0, 255);
			const auto g = std::clamp(std::stoi(matches[2]), 0, 255);
			const auto b = std::clamp(std::stoi(matches[3]), 0, 255);

			return Color::FromComponentRGB(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
		}

		static const std::map<std::string, Color> kKnownNames
			{
				{ "Red", Palette::kRed },
				{ "Green", Palette::kGreen },
				{ "Blue", Palette::kBlue },
				{ "Black", Palette::kBlack },
				{ "White", Palette::kWhite },
				{ "Cyan", Palette::kCyan },
				{ "Yellow", Palette::kYellow },
				{ "Magenta", Palette::kMagenta },
			};
		if (kKnownNames.contains(value))
			return kKnownNames.at(value);

		return Color(0, 0, 0);
	}

	Color TryParseColor(const fkyaml::node& node, const std::string& property, Color missingValue)
	{
		if (! node.contains(property))
			return missingValue;

		return ParseColor(node[property].get_value<std::string>());
	}

	Vector ParseVector(std::string value)
	{
		value = TrimWhitespace(value);

		static const std::regex normColorRegex("Vector\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
		if (std::smatch matches; std::regex_search(value, matches, normColorRegex))
		{
			const auto x = std::stod(matches[1]);
			const auto y = std::stod(matches[2]);
			const auto z = std::stod(matches[3]);

			return Vector(x, y, z);
		}

		return Vector(0, 0, 0);
	}

	Vector TryParseVector(const fkyaml::node& node, const std::string& property, Vector missingValue)
	{
		if (! node.contains(property))
			return missingValue;

		return ParseVector(node[property].get_value<std::string>());
	}

	double TryParseDouble(const fkyaml::node& node, const std::string& property, double missingValue)
	{
		if (! node.contains(property))
			return missingValue;

		return node[property].is_float_number() ? node[property].get_value<double>() : node[property].get_value<int64_t>();
	}

	Camera ParseCamera(const fkyaml::node& node)
	{
		Vector position = TryParseVector(node, "position", StandardVectors::kOrigin);
		Vector target = TryParseVector(node, "target", StandardVectors::kUnitZ);
		double viewWidth = TryParseDouble(node, "width", 4.0);
		double viewHeight = TryParseDouble(node, "height", 9.0 / 4.0);

		return Camera(position, target, viewWidth, viewHeight);
	}

	std::shared_ptr<Texture> ParseCheckerboardTexture(const fkyaml::node& node)
	{
		Color color1 = TryParseColor(node, "color1", Palette::kMagenta);
		Color color2 = TryParseColor(node, "color2", Palette::kYellow);
		double rowsCols = TryParseDouble(node, "rowsCols", 2);

		return std::make_shared<CheckerboardTexture>(color1, color2, static_cast<uint8_t>(rowsCols));
	}

	std::shared_ptr<Texture> ParseImageTexture(const fkyaml::node& node)
	{
		std::string path = node["path"].get_value<std::string>();

		return MakeImageTexture(path);
	}

	std::shared_ptr<Texture> ParseSolidTexture(const fkyaml::node& node)
	{
		Color color = TryParseColor(node, "color", Palette::kWhite);

		return std::make_shared<SolidTexture>(color);
	}

	std::shared_ptr<Texture> ParseTexture(const fkyaml::node& node)
	{
		const auto type = node["type"].get_value<std::string>();

		if (type == "Checkerboard")
			return ParseCheckerboardTexture(node);
		else if (type == "Image")
			return ParseImageTexture(node);
		else if (type == "Solid")
			return ParseSolidTexture(node);
		else
			throw std::runtime_error("Unknown texture type '" + type + "' specified in scene YAML file");
	}

	std::shared_ptr<Material> ParseDebugMaterial(const fkyaml::node& node)
	{
		const auto type = node["type"].get_value<std::string>();

		if (type == "Normal")
			return std::make_shared<DebugMaterial>(DebugMaterial::Type::Normal);
		else if (type == "UV")
			return std::make_shared<DebugMaterial>(DebugMaterial::Type::UV);
		else
			throw std::runtime_error("Unknown debug material type '" + type + "' specified in scene YAML file");
	}

	std::shared_ptr<Material> ParseDielectricMaterial(const fkyaml::node& node)
	{
		std::shared_ptr<Texture> texture = ParseTexture(node["texture"]);
		double refractionIndex = TryParseDouble(node, "refrectionIndex", 1.0);

		return std::make_shared<DielectricMaterial>(std::move(texture), refractionIndex);
	}

	std::shared_ptr<Material> ParseDiffuseMaterial(const fkyaml::node& node)
	{
		std::shared_ptr<Texture> texture = ParseTexture(node["texture"]);

		return std::make_shared<DiffuseMaterial>(std::move(texture));
	}

	std::shared_ptr<Material> ParseLightMaterial(const fkyaml::node& node)
	{
		std::shared_ptr<Texture> texture = ParseTexture(node["texture"]);

		return std::make_shared<LightMaterial>(std::move(texture));
	}

	std::shared_ptr<Material> ParseReflectiveMaterial(const fkyaml::node& node)
	{
		std::shared_ptr<Texture> texture = ParseTexture(node["texture"]);
		double polish = TryParseDouble(node, "refrectionIndex", 1.0);

		return std::make_shared<ReflectiveMaterial>(std::move(texture), polish);
	}

	std::shared_ptr<Material> ParseMaterial(const fkyaml::node& node)
	{
		const auto type = node["type"].get_value<std::string>();

		if (type == "Debug")
			return ParseDebugMaterial(node);
		else if (type == "Dielectric")
			return ParseDielectricMaterial(node);
		else if (type == "Diffuse")
			return ParseDiffuseMaterial(node);
		else if (type == "Light")
			return ParseLightMaterial(node);
		else if (type == "Reflective")
			return ParseReflectiveMaterial(node);
		else
			throw std::runtime_error("Unknown material type '" + type + "' specified in scene YAML file");
	}

	std::shared_ptr<Object> ParseBoxObject(const fkyaml::node& node)
	{
		Vector position = TryParseVector(node, "position", StandardVectors::kOrigin);
		Vector rotation = TryParseVector(node, "rotation", Vector(0, 0, 0));
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		Vector size = TryParseVector(node, "size", Vector(1, 1, 1));

		return std::make_shared<BoxObject>(position, rotation, std::move(material), size);
	}

	std::shared_ptr<Object> ParsePlaneObject(const fkyaml::node& node)
	{
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		Vector normal = TryParseVector(node, "normal", StandardVectors::kUnitY);
		double distance = TryParseDouble(node, "distance", 0.0);
		double uvScaleFactor = TryParseDouble(node, "scale", 1.0);

		return std::make_shared<PlaneObject>(std::move(material), normal, distance, uvScaleFactor);
	}

	std::shared_ptr<Object> ParseMeshObject(const fkyaml::node& node)
	{
		Vector position = TryParseVector(node, "position", StandardVectors::kOrigin);
		Vector rotation = TryParseVector(node, "rotation", Vector(0, 0, 0));
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		std::string path = node["path"].get_value<std::string>();
		double scaleFactor = TryParseDouble(node, "scale", 1.0);

		return std::make_shared<MeshObject>(position, rotation, std::move(material), MakeObjectMesh(path, scaleFactor));
	}

	std::shared_ptr<Object> ParseSphereObject(const fkyaml::node& node)
	{
		Vector position = TryParseVector(node, "position", StandardVectors::kOrigin);
		Vector rotation = TryParseVector(node, "rotation", Vector(0, 0, 0));
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		double radius = TryParseDouble(node, "radius", 1.0);

		return std::make_shared<SphereObject>(position, rotation, std::move(material), radius);
	}

	std::vector<std::shared_ptr<Object>> ParseObjects(const fkyaml::node& node)
	{
		std::vector<std::shared_ptr<Object>> objects;

		for (const auto& yamlObject : node)
		{
			const auto type = yamlObject["type"].get_value<std::string>();

			if (type == "Box")
				objects.push_back(ParseBoxObject(yamlObject));
			else if (type == "Mesh")
				objects.push_back(ParseMeshObject(yamlObject));
			else if (type == "Plane")
				objects.push_back(ParsePlaneObject(yamlObject));
			else if (type == "Sphere")
				objects.push_back(ParseSphereObject(yamlObject));
			else
				throw std::runtime_error("Unknown object type '" + type + "' in scene YAML file");
		}

		return objects;
	}
}

Scene SceneLoader::Load(const std::string& path)
{
	Scene scene;

	std::ifstream config(path);
	if (! config)
		throw std::runtime_error("Failed to read scene YAML file '" + path + "'");

	fkyaml::node yamlRoot = fkyaml::node::deserialize(config);

	if (! yamlRoot.contains("scene"))
		throw std::runtime_error("No scene root node in scene YAML file '" + path + "'");

	fkyaml::node yamlScene = yamlRoot["scene"];

	scene.background = TryParseColor(yamlScene, "background", Palette::kBlack);

	if (yamlScene.contains("camera"))
		scene.camera = ParseCamera(yamlScene["camera"]);

	if (yamlScene.contains("objects"))
		scene.objects = ParseObjects(yamlScene["objects"]);

	return scene;
}
