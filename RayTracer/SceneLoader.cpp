#include "SceneLoader.hpp"

#include "Engine/Color.hpp"
#include "Engine/Material.hpp"
#include "Engine/Material/DebugMaterial.hpp"
#include "Engine/Material/DielectricMaterial.hpp"
#include "Engine/Material/DiffuseMaterial.hpp"
#include "Engine/Material/LightMaterial.hpp"
#include "Engine/Material/ReflectiveMaterial.hpp"
#include "Engine/Mesh.hpp"
#include "Engine/Object.hpp"
#include "Engine/Object/BoxObject.hpp"
#include "Engine/Object/MeshObject.hpp"
#include "Engine/Object/PlaneObject.hpp"
#include "Engine/Object/SphereObject.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"
#include "Engine/Texture/ImageTexture.hpp"
#include "Engine/Texture/SolidTexture.hpp"
#include "Engine/Vector.hpp"

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

	std::optional<Color> ParseColor(const fkyaml::node& node, const std::string& property)
	{
		if (! node.contains(property))
			return std::nullopt;

		std::string value = TrimWhitespace(node[property].get_value<std::string>());

		static const std::regex normColorRegex("Color\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
		if (std::smatch matches; std::regex_search(value, matches, normColorRegex))
		{
			const auto r = std::stod(matches[1]);
			const auto g = std::stod(matches[2]);
			const auto b = std::stod(matches[3]);

			return Color(r, g, b);
		}

		static const std::regex componentColorRegex("Color8Bit\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
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

		throw std::runtime_error("Unknown color type '" + value + "' specified in scene YAML file");
	}

	std::optional<Vector> ParseVector(const fkyaml::node& node, const std::string& property)
	{
		if (! node.contains(property))
			return std::nullopt;

		std::string value = TrimWhitespace(node[property].get_value<std::string>());

		static const std::regex vectorRegex("Vector\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
		if (std::smatch matches; std::regex_search(value, matches, vectorRegex))
		{
			const auto x = std::stod(matches[1]);
			const auto y = std::stod(matches[2]);
			const auto z = std::stod(matches[3]);

			return Vector(x, y, z);
		}

		static const std::regex degreesVectorRegex("VectorDegrees\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
		if (std::smatch matches; std::regex_search(value, matches, degreesVectorRegex))
		{
			const auto x = std::stod(matches[1]) * (std::numbers::pi / 180);
			const auto y = std::stod(matches[2]) * (std::numbers::pi / 180);
			const auto z = std::stod(matches[3]) * (std::numbers::pi / 180);

			return Vector(x, y, z);
		}

		static const std::map<std::string, Vector> kKnownNames
			{
				{ "Zero", Vector(0, 0, 0) },
				{ "Origin", StandardVectors::kOrigin },
				{ "UnitX", StandardVectors::kUnitX },
				{ "UnitY", StandardVectors::kUnitY },
				{ "UnitZ", StandardVectors::kUnitZ },
			};
		if (kKnownNames.contains(value))
			return kKnownNames.at(value);

		throw std::runtime_error("Unknown vector type '" + value + "' specified in scene YAML file");
	}

 	std::optional<double> ParseDouble(const fkyaml::node& node, const std::string& property)
	{
        if (! node.contains(property))
            return std::nullopt;

		return node[property].is_float_number() ? node[property].get_value<double>() : node[property].get_value<int64_t>();
	}

	std::optional<Camera> ParseCamera(const fkyaml::node& node, const std::string& property)
	{
		if (! node.contains(property))
			return std::nullopt;

		auto cameraNode = node.at(property);

		Vector position = ParseVector(cameraNode, "position").value_or(StandardVectors::kOrigin);
		Vector target = ParseVector(cameraNode, "target").value_or(StandardVectors::kUnitZ);
		double viewWidth = ParseDouble(cameraNode, "width").value_or(4.0);
		double viewHeight = ParseDouble(cameraNode, "height").value_or(9.0 / 4.0);

		return Camera(position, target, viewWidth, viewHeight);
	}

	std::shared_ptr<Texture> ParseCheckerboardTexture(const fkyaml::node& node)
	{
		Color color1 = ParseColor(node, "color1").value_or(Palette::kMagenta);
		Color color2 = ParseColor(node, "color2").value_or(Palette::kYellow);
		double rowsCols = ParseDouble(node, "rowsCols").value_or(2);

		return std::make_shared<CheckerboardTexture>(color1, color2, static_cast<uint8_t>(rowsCols));
	}

	std::shared_ptr<Texture> ParseImageTexture(const fkyaml::node& node)
	{
		std::string path = node.at("path").get_value<std::string>();

		return MakeImageTexture(path);
	}

	std::shared_ptr<Texture> ParseSolidTexture(const fkyaml::node& node)
	{
		Color color = ParseColor(node, "color").value_or(Palette::kWhite);

		return std::make_shared<SolidTexture>(color);
	}

	std::shared_ptr<Texture> ParseTexture(const fkyaml::node& node)
	{
		const auto type = node.at("type").get_value<std::string>();

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
		const auto mode = node.at("mode").get_value<std::string>();

		if (mode == "Normal")
			return std::make_shared<DebugMaterial>(DebugMaterial::Mode::Normal);
		else if (mode == "UV")
			return std::make_shared<DebugMaterial>(DebugMaterial::Mode::UV);
		else
			throw std::runtime_error("Unknown debug material mode '" + mode + "' specified in scene YAML file");
	}

	std::shared_ptr<Material> ParseDielectricMaterial(const fkyaml::node& node)
	{
		std::shared_ptr<Texture> texture = ParseTexture(node["texture"]);
		double refractionIndex = ParseDouble(node, "refractionIndex").value_or(1.0);

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
		double polish = ParseDouble(node, "polish").value_or(1.0);

		return std::make_shared<ReflectiveMaterial>(std::move(texture), polish);
	}

	std::shared_ptr<Material> ParseMaterial(const fkyaml::node& node)
	{
		const auto type = node.at("type").get_value<std::string>();

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
		Vector position = ParseVector(node, "position").value_or(StandardVectors::kOrigin);
		Vector rotation = ParseVector(node, "rotation").value_or(Vector(0, 0, 0));
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		Vector size = ParseVector(node, "size").value_or(Vector(1, 1, 1));

		return std::make_shared<BoxObject>(position, rotation, std::move(material), size);
	}

	std::shared_ptr<Object> ParsePlaneObject(const fkyaml::node& node)
	{
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		Vector normal = ParseVector(node, "normal").value_or(StandardVectors::kUnitY);
		double distance = ParseDouble(node, "distance").value_or(0.0);
		double uvScaleFactor = ParseDouble(node, "scale").value_or(1.0);

		return std::make_shared<PlaneObject>(std::move(material), normal, distance, uvScaleFactor);
	}

	std::shared_ptr<Object> ParseMeshObject(const fkyaml::node& node)
	{
		Vector position = ParseVector(node, "position").value_or(StandardVectors::kOrigin);
		Vector rotation = ParseVector(node, "rotation").value_or(Vector(0, 0, 0));
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		std::string path = node["path"].get_value<std::string>();
		double scaleFactor = ParseDouble(node, "scale").value_or(1.0);

		return std::make_shared<MeshObject>(position, rotation, std::move(material), MakeObjectMesh(path, scaleFactor));
	}

	std::shared_ptr<Object> ParseSphereObject(const fkyaml::node& node)
	{
		Vector position = ParseVector(node, "position").value_or(StandardVectors::kOrigin);
		Vector rotation = ParseVector(node, "rotation").value_or(Vector(0, 0, 0));
		std::shared_ptr<Material> material = ParseMaterial(node["material"]);
		double radius = ParseDouble(node, "radius").value_or(1.0);

		return std::make_shared<SphereObject>(position, rotation, std::move(material), radius);
	}

	std::vector<std::shared_ptr<Object>> ParseObjects(const fkyaml::node& node, const std::string& property)
	{
		if (! node.contains(property))
			return {};

		auto objectsNode = node.at(property);

		std::vector<std::shared_ptr<Object>> objects;

		for (const auto& yamlObject : objectsNode)
		{
			const auto type = yamlObject.at("type").get_value<std::string>();

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

	fkyaml::node rootNode = fkyaml::node::deserialize(config);
	fkyaml::node sceneNode = rootNode.at("scene");

	scene.background = ParseColor(sceneNode, "background").value_or(Palette::kBlack);
	scene.camera = ParseCamera(sceneNode, "camera").value_or(Camera());
	scene.objects = ParseObjects(sceneNode, "objects");

	return scene;
}