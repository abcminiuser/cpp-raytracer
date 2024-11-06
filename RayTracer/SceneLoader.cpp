#include "SceneLoader.hpp"

#include "Engine/Material/DebugMaterial.hpp"
#include "Engine/Material/DielectricMaterial.hpp"
#include "Engine/Material/DiffuseMaterial.hpp"
#include "Engine/Material/LightMaterial.hpp"
#include "Engine/Material/ReflectiveMaterial.hpp"
#include "Engine/Object/BoxObject.hpp"
#include "Engine/Object/MeshObject.hpp"
#include "Engine/Object/PlaneObject.hpp"
#include "Engine/Object/SphereObject.hpp"
#include "Engine/Texture/CheckerboardTexture.hpp"
#include "Engine/Texture/ImageTexture.hpp"
#include "Engine/Texture/SolidTexture.hpp"

#include <SFML/Graphics.hpp>

#include <OBJ_Loader.h>

#include <fstream>
#include <numbers>
#include <regex>
#include <stdexcept>
#include <stdint.h>

namespace
{
	constexpr double DegreesToRadians(double degrees)
	{
		return degrees * (std::numbers::pi / 180);
	}

	std::string TrimWhitespace(std::string value)
	{
		if (auto pos = value.find_first_not_of(" \t"); pos != std::string::npos)
			value = value.substr(pos);

		if (auto pos = value.find_last_not_of(" \t"); pos != std::string::npos)
			value = value.substr(0, pos + 1);

		return value;
	}

	double DoubleFromString(std::string str)
	{
		str = TrimWhitespace(str);

		char* parsedEnd;
		double value = std::strtod(str.data(), &parsedEnd);

		if (parsedEnd != (str.data() + str.size()))
			throw std::runtime_error("Failed to parse double '" + str + "' in scene YAML file");

		return value;
	}
}

Scene SceneLoader::load(const std::string& path)
{
	std::ifstream config(path);
	if (! config)
		throw std::runtime_error("Failed to read scene YAML file '" + path + "'");

	fkyaml::node rootNode = fkyaml::node::deserialize(config);
	fkyaml::node sceneNode = rootNode.at("scene");

	// Clear cache after each load; we want a reload of the same scene to reload
	// the file on disk at least once, in case it's changed since we last used it.
	clearCache();

	return
		Scene
		{
			.background = tryParseColor(sceneNode, "background").value_or(Palette::kBlack),
			.camera = tryParseCamera(sceneNode, "camera").value_or(Camera()),
			.objects = parseObjects(sceneNode, "objects"),
			.samplesPerPixel = std::max<uint32_t>(static_cast<uint32_t>(tryParseDouble(sceneNode, "samplesPerPixel").value_or(100)), 1)
		};
}

void SceneLoader::clearCache()
{
	m_cache = {};
}

std::vector<std::shared_ptr<Object>> SceneLoader::parseObjects(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return {};

	const auto objectsNode = node.at(property);

	std::vector<std::shared_ptr<Object>> objects;

	for (const auto& yamlObject : objectsNode)
	{
		const auto type = yamlObject.at("type").get_value<std::string>();

		if (type == "Box")
			objects.push_back(parseBoxObject(yamlObject));
		else if (type == "Mesh")
			objects.push_back(parseMeshObject(yamlObject));
		else if (type == "Plane")
			objects.push_back(parsePlaneObject(yamlObject));
		else if (type == "Sphere")
			objects.push_back(parseSphereObject(yamlObject));
		else
			throw std::runtime_error("Unknown object type '" + type + "' in scene YAML file");
	}

	return objects;
}

std::shared_ptr<Object> SceneLoader::parseBoxObject(const fkyaml::node& node)
{
	auto transform			= tryParseTransform(node, "transform").value_or(Transform());
	auto material			= parseMaterial(node, "material");

	return std::make_shared<BoxObject>(transform, std::move(material));
}

std::shared_ptr<Object> SceneLoader::parseMeshObject(const fkyaml::node& node)
{
	auto transform			= tryParseTransform(node, "transform").value_or(Transform());
	auto material			= parseMaterial(node, "material");
	auto path				= node.at("path").get_value<std::string>();

	return std::make_shared<MeshObject>(transform, std::move(material), makeObjectMesh(path));
}

std::shared_ptr<Object> SceneLoader::parsePlaneObject(const fkyaml::node& node)
{
	auto transform			= tryParseTransform(node, "transform").value_or(Transform());
	auto material			= parseMaterial(node, "material");

	return std::make_shared<PlaneObject>(transform, std::move(material));
}

std::shared_ptr<Object> SceneLoader::parseSphereObject(const fkyaml::node& node)
{
	auto transform			= tryParseTransform(node, "transform").value_or(Transform());
	auto material			= parseMaterial(node, "material");

	return std::make_shared<SphereObject>(transform, std::move(material));
}

std::shared_ptr<Texture> SceneLoader::parseTexture(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return nullptr;

	const auto textureNode = node.at(property);

	const auto type = textureNode.at("type").get_value<std::string>();

	if (type == "Checkerboard")
		return parseCheckerboardTexture(textureNode);
	else if (type == "Image")
		return parseImageTexture(textureNode);
	else if (type == "Solid")
		return parseSolidTexture(textureNode);
	else
		throw std::runtime_error("Unknown texture type '" + type + "' specified in scene YAML file");
}

std::shared_ptr<Texture> SceneLoader::parseCheckerboardTexture(const fkyaml::node& node)
{
	auto color1				= tryParseColor(node, "color1").value_or(Palette::kMagenta);
	auto color2				= tryParseColor(node, "color2").value_or(Palette::kYellow);
	auto rowsCols			= tryParseDouble(node, "rowsCols").value_or(2);
	auto interpolation		= tryParseInterpolation(node, "interpolation").value_or(Texture::Interpolation::Bilinear);

	return std::make_shared<CheckerboardTexture>(interpolation, color1, color2, static_cast<uint8_t>(rowsCols));
}

std::shared_ptr<Texture> SceneLoader::parseImageTexture(const fkyaml::node& node)
{
	auto path				= node.at("path").get_value<std::string>();
	auto multiplier			= tryParseColor(node, "multiplier").value_or(Palette::kWhite);
	auto interpolation		= tryParseInterpolation(node, "interpolation").value_or(Texture::Interpolation::Bilinear);

	return makeImageTexture(path, multiplier, interpolation);
}

std::shared_ptr<Texture> SceneLoader::parseSolidTexture(const fkyaml::node& node)
{
	auto color				= tryParseColor(node, "color").value_or(Palette::kWhite);

	return std::make_shared<SolidTexture>(color);
}

std::shared_ptr<Material> SceneLoader::parseMaterial(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return nullptr;

	const auto materialNode = node.at(property);

	const auto type = materialNode.at("type").get_value<std::string>();

	if (type == "Debug")
		return parseDebugMaterial(materialNode);
	else if (type == "Dielectric")
		return parseDielectricMaterial(materialNode);
	else if (type == "Diffuse")
		return parseDiffuseMaterial(materialNode);
	else if (type == "Light")
		return parseLightMaterial(materialNode);
	else if (type == "Reflective")
		return parseReflectiveMaterial(materialNode);
	else
		throw std::runtime_error("Unknown material type '" + type + "' specified in scene YAML file");
}

std::shared_ptr<Material> SceneLoader::parseDebugMaterial(const fkyaml::node& node)
{
	const auto mode = node.at("mode").get_value<std::string>();

	if (mode == "Normal")
		return std::make_shared<DebugMaterial>(DebugMaterial::Mode::Normal);
	else if (mode == "UV")
		return std::make_shared<DebugMaterial>(DebugMaterial::Mode::UV);
	else
		throw std::runtime_error("Unknown debug material mode '" + mode + "' specified in scene YAML file");
}

std::shared_ptr<Material> SceneLoader::parseDielectricMaterial(const fkyaml::node& node)
{
	auto texture			= parseTexture(node, "texture");
	auto normals			= parseTexture(node, "normals");
	auto refractionIndex	= tryParseDouble(node, "refractionIndex").value_or(1.0);

	return std::make_shared<DielectricMaterial>(std::move(texture), std::move(normals), refractionIndex);
}

std::shared_ptr<Material> SceneLoader::parseDiffuseMaterial(const fkyaml::node& node)
{
	auto texture			= parseTexture(node, "texture");
	auto normals			= parseTexture(node, "normals");

	return std::make_shared<DiffuseMaterial>(std::move(texture), std::move(normals));
}

std::shared_ptr<Material> SceneLoader::parseLightMaterial(const fkyaml::node& node)
{
	auto texture			= parseTexture(node, "texture");
	auto normals			= parseTexture(node, "normals");

	return std::make_shared<LightMaterial>(std::move(texture), std::move(normals));
}

std::shared_ptr<Material> SceneLoader::parseReflectiveMaterial(const fkyaml::node& node)
{
	auto texture			= parseTexture(node, "texture");
	auto normals			= parseTexture(node, "normals");
	auto polish				= tryParseDouble(node, "polish").value_or(1.0);

	return std::make_shared<ReflectiveMaterial>(std::move(texture), std::move(normals), polish);
}

std::optional<Color> SceneLoader::tryParseColor(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	std::string value = TrimWhitespace(node[property].get_value<std::string>());

	static const std::regex normColorRegex("Color\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
	if (std::smatch matches; std::regex_match(value, matches, normColorRegex))
	{
		auto r = DoubleFromString(matches[1]);
		auto g = DoubleFromString(matches[2]);
		auto b = DoubleFromString(matches[3]);

		return Color(r, g, b);
	}

	static const std::regex rgb888ColorRegex("ColorRGB888\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
	if (std::smatch matches; std::regex_match(value, matches, rgb888ColorRegex))
	{
		auto r = std::clamp(DoubleFromString(matches[1]), 0.0, 255.0);
		auto g = std::clamp(DoubleFromString(matches[2]), 0.0, 255.0);
		auto b = std::clamp(DoubleFromString(matches[3]), 0.0, 255.0);

		return Color::FromRGB888(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b));
	}

	static const std::unordered_map<std::string, Color> kKnownNames
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

std::optional<Vector> SceneLoader::tryParseVector(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	std::string value = TrimWhitespace(node[property].get_value<std::string>());

	static const std::regex vectorRegex("Vector\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
	if (std::smatch matches; std::regex_match(value, matches, vectorRegex))
	{
		auto x = DoubleFromString(matches[1]);
		auto y = DoubleFromString(matches[2]);
		auto z = DoubleFromString(matches[3]);

		return Vector(x, y, z);
	}

	static const std::regex degreesVectorRegex("VectorDegrees\\(" "([^\\,]+)" "," "([^\\,]+)" "," "([^\\)]+)" "\\)");
	if (std::smatch matches; std::regex_match(value, matches, degreesVectorRegex))
	{
		auto x = DegreesToRadians(DoubleFromString(matches[1]));
		auto y = DegreesToRadians(DoubleFromString(matches[2]));
		auto z = DegreesToRadians(DoubleFromString(matches[3]));

		return Vector(x, y, z);
	}

	static const std::unordered_map<std::string, Vector> kKnownNames
		{
			{ "Zero", Vector(0, 0, 0) },
			{ "Origin", StandardVectors::kOrigin },
			{ "UnitX", StandardVectors::kUnitX },
			{ "UnitY", StandardVectors::kUnitY },
			{ "UnitZ", StandardVectors::kUnitZ },
			{ "Unit", StandardVectors::kUnit },
		};
	if (kKnownNames.contains(value))
		return kKnownNames.at(value);

	throw std::runtime_error("Unknown vector type '" + value + "' specified in scene YAML file");
}

std::optional<Texture::Interpolation> SceneLoader::tryParseInterpolation(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	std::string value = TrimWhitespace(node[property].get_value<std::string>());

	static const std::unordered_map<std::string, Texture::Interpolation> kKnownNames
		{
			{ "NearestNeighbor", Texture::Interpolation::NearestNeighbor },
			{ "Bilinear", Texture::Interpolation::Bilinear },
		};
	if (kKnownNames.contains(value))
		return kKnownNames.at(value);

	throw std::runtime_error("Unknown interpolation type '" + value + "' specified in scene YAML file");
}

std::optional<double> SceneLoader::tryParseAspectRatio(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	const auto& valueNode = node[property];

	if (valueNode.is_float_number())
	{
		return valueNode.get_value<double>();
	}
	else
	{
		std::string value = TrimWhitespace(node[property].get_value<std::string>());

		static const std::regex aspectRatioRegex("([^\\,]+)" ":" "([^\\,]+)");
		if (std::smatch matches; std::regex_match(value, matches, aspectRatioRegex))
		{
			auto numerator		= DoubleFromString(matches[1]);
			auto denominator	= DoubleFromString(matches[2]);

			return numerator / denominator;
		}
	}

	throw std::runtime_error("Unknown aspect ratio type specified in scene YAML file");
}

std::optional<double> SceneLoader::tryParseDouble(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	const auto& valueNode = node[property];

	if (valueNode.is_float_number())
		return valueNode.get_value<double>();
	else if (valueNode.is_integer())
		return static_cast<double>(valueNode.get_value<int64_t>());
	else
		return DoubleFromString(valueNode.get_value<std::string>());
}

std::optional<Camera> SceneLoader::tryParseCamera(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	const auto cameraNode = node.at(property);

	auto position			= tryParseVector(cameraNode, "position").value_or(StandardVectors::kOrigin);
	auto target				= tryParseVector(cameraNode, "target").value_or(StandardVectors::kUnitZ);
	auto orientation		= tryParseVector(cameraNode, "orientation").value_or(StandardVectors::kUnitY);
	auto aspectRatio		= tryParseAspectRatio(cameraNode, "aspectRatio").value_or(16.0 / 9.0);
	auto verticalFov		= DegreesToRadians(tryParseDouble(cameraNode, "verticalFov").value_or(90));
	auto focusDistance		= tryParseDouble(cameraNode, "focusDistance").value_or(1.0);
	auto aperture			= tryParseDouble(cameraNode, "aperture").value_or(0.0);

	return Camera(position, target, orientation, aspectRatio, verticalFov, focusDistance, aperture);
}

std::optional<Transform> SceneLoader::tryParseTransform(const fkyaml::node& node, const std::string& property)
{
	if (! node.contains(property))
		return std::nullopt;

	const auto transformNode = node.at(property);

	auto position			= tryParseVector(transformNode, "position").value_or(StandardVectors::kOrigin);
	auto rotation			= tryParseVector(transformNode, "rotation").value_or(StandardVectors::kZero);
	auto scale				= tryParseVector(transformNode, "scale").value_or(StandardVectors::kUnit);

	Transform transform;
	transform.setPosition(position);
	transform.setRotation(rotation);
	transform.setScale(scale);

	return transform;
}

std::shared_ptr<ImageTexture> SceneLoader::makeImageTexture(const std::string& path, const Color& multiplier, Texture::Interpolation interpolation)
{
	auto cachedEntry = m_cache.imageTextures.find(path);
	if (cachedEntry != m_cache.imageTextures.end())
		return cachedEntry->second;

	sf::Image imageTexture;
	if (! imageTexture.loadFromFile(path))
		throw std::runtime_error("Failed to load image: " + path);

	const auto	dimensions	= imageTexture.getSize();
	const auto* pixels		= imageTexture.getPixelsPtr();

	return std::make_shared<ImageTexture>(dimensions.x, dimensions.y, multiplier, interpolation, reinterpret_cast<const uint32_t*>(pixels));
}

std::shared_ptr<Mesh> SceneLoader::makeObjectMesh(const std::string& path)
{
	auto cachedEntry = m_cache.meshes.find(path);
	if (cachedEntry != m_cache.meshes.end())
		return cachedEntry->second;

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
					.position	= Vector(static_cast<double>(v.Position.X), static_cast<double>(v.Position.Y), static_cast<double>(v.Position.Z)),
					.normal		= Vector(static_cast<double>(v.Normal.X), static_cast<double>(v.Normal.Y), static_cast<double>(v.Normal.Z)),
					.texture	= Vector(static_cast<double>(v.TextureCoordinate.X), static_cast<double>(v.TextureCoordinate.Y), 0.0)
				}
			);
		}

		for (size_t i = 0; i < mesh.Indices.size(); i += 3)
			triangles.emplace_back(Triangle{ meshStartPos + mesh.Indices[i + 0], meshStartPos + mesh.Indices[i + 1], meshStartPos + mesh.Indices[i + 2] });
	}

	return std::make_shared<Mesh>(std::move(vertices), std::move(triangles));
}
