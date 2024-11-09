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

#include <fkYAML/node.hpp>

#include <fstream>
#include <numbers>
#include <regex>
#include <stdexcept>
#include <stddef.h>
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

struct SceneLoader::NodeHolder
{
public:
						NodeHolder(const fkyaml::node& node, const std::string& path)
							: m_node(node)
							, m_path(path)
	{

	}

						NodeHolder(const NodeHolder& parent, const std::string& property, bool required = false)
							: m_path(parent.path())
	{
		if (! property.empty())
		{
			if (parent.node().contains(property))
			{
				m_node = parent.node().at(property);
				m_path += std::string("/") + property;
			}
			else if (required)
			{
				throw std::runtime_error("Failed to find mandatory property '" + property + "' (" + m_path + ")");
			}
		}
		else
		{
			m_node = parent.node();
		}
	}

	NodeHolder			getChild(const std::string& property, bool required = false) const
	{
		return NodeHolder(*this, property, required);
	}

	template <typename T>
	T					getValue() const
	{
		try
		{
			return node().get_value<T>();
		}
		catch (const fkyaml::exception& e)
		{
			throw std::runtime_error("Failed to read property: " + std::string(e.what()) + " (" + m_path + ")");
		}
	}

	template <typename T>
	std::optional<T>	tryGetValue() const
	{
		try
		{
			return node().get_value<T>();
		}
		catch (const fkyaml::exception&)
		{
			return std::nullopt;
		}
	}

	operator			bool() const
	{
		return ! m_node.is_null();
	}

	const fkyaml::node&	node() const
	{
		return m_node;
	}

	const std::string&	path() const
	{
		return m_path;
	}

private:
	fkyaml::node	m_node;
	std::string		m_path;
};

Scene SceneLoader::load(const std::string& path)
{
	std::ifstream config(path);
	if (! config)
		throw std::runtime_error("Failed to read scene YAML file '" + path + "'");

	NodeHolder rootNode(fkyaml::node::deserialize(config), "");

	Scene scene = parseScene(rootNode.getChild("scene", true));

	// Clear cache after each load; we want a reload of the same scene to reload
	// the file on disk at least once, in case it's changed since we last used it.
	m_cache = {};

	return scene;
}

Scene SceneLoader::parseScene(const NodeHolder& node)
{
	if (! node)
		return {};

	return
		{
			.background			= parseTexture(node.getChild("background")),
			.camera				= tryParseCamera(node.getChild("camera")).value_or(Camera()),
			.objects			= parseObjects(node.getChild("objects")),
			.samplesPerPixel	= std::max<uint32_t>(static_cast<uint32_t>(tryParseDouble(node.getChild("samplesPerPixel")).value_or(100)), 1)
		};
}

std::vector<std::shared_ptr<Object>> SceneLoader::parseObjects(const NodeHolder& node)
{
	if (! node)
		return {};

	std::vector<std::shared_ptr<Object>> objects;

	for (const auto& object : node.node())
		objects.push_back(parseObject(NodeHolder(object, node.path() + "[" + std::to_string(objects.size()) + "]")));

	return objects;
}

std::shared_ptr<Object> SceneLoader::parseObject(const NodeHolder& node)
{
	const auto type = node.getChild("type", true).getValue<std::string>();

	if (type == "Box")
		return parseBoxObject(node);
	else if (type == "Mesh")
		return parseMeshObject(node);
	else if (type == "Plane")
		return parsePlaneObject(node);
	else if (type == "Sphere")
		return parseSphereObject(node);
	else
		throw std::runtime_error("Unknown object type '" + type + "' in scene YAML file (" + node.path() + ")");
}

std::shared_ptr<Object> SceneLoader::parseBoxObject(const NodeHolder& node)
{
	auto transform			= tryParseTransform(node.getChild("transform")).value_or(Transform());
	auto material			= parseMaterial(node.getChild("material"));

	return std::make_shared<BoxObject>(transform, std::move(material));
}

std::shared_ptr<Object> SceneLoader::parseMeshObject(const NodeHolder& node)
{
	auto transform			= tryParseTransform(node.getChild("transform")).value_or(Transform());
	auto material			= parseMaterial(node.getChild("material"));
	auto path				= node.getChild("path", true).getValue<std::string>();

	return std::make_shared<MeshObject>(transform, std::move(material), makeObjectMesh(path));
}

std::shared_ptr<Object> SceneLoader::parsePlaneObject(const NodeHolder& node)
{
	auto transform			= tryParseTransform(node.getChild("transform")).value_or(Transform());
	auto material			= parseMaterial(node.getChild("material"));

	return std::make_shared<PlaneObject>(transform, std::move(material));
}

std::shared_ptr<Object> SceneLoader::parseSphereObject(const NodeHolder& node)
{
	auto transform			= tryParseTransform(node.getChild("transform")).value_or(Transform());
	auto material			= parseMaterial(node.getChild("material"));

	return std::make_shared<SphereObject>(transform, std::move(material));
}

std::shared_ptr<Texture> SceneLoader::parseTexture(const NodeHolder& node)
{
	if (! node)
		return nullptr;

	const auto type = node.getChild("type", true).getValue<std::string>();

	if (type == "Checkerboard")
		return parseCheckerboardTexture(node);
	else if (type == "Image")
		return parseImageTexture(node);
	else if (type == "Solid")
		return parseSolidTexture(node);
	else
		throw std::runtime_error("Unknown texture type '" + type + "' in scene YAML file (" + node.path() + ")");
}

std::shared_ptr<Texture> SceneLoader::parseCheckerboardTexture(const NodeHolder& node)
{
	auto color1				= tryParseColor(node.getChild("color1")).value_or(Palette::kMagenta);
	auto color2				= tryParseColor(node.getChild("color2")).value_or(Palette::kYellow);
	auto rowsCols			= tryParseDouble(node.getChild("rowsCols")).value_or(2);
	auto interpolation		= tryParseInterpolation(node.getChild("interpolation")).value_or(Texture::Interpolation::Bilinear);

	return std::make_shared<CheckerboardTexture>(interpolation, color1, color2, static_cast<uint8_t>(rowsCols));
}

std::shared_ptr<Texture> SceneLoader::parseImageTexture(const NodeHolder& node)
{
	auto path				= node.getChild("path", true).getValue<std::string>();
	auto multiplier			= tryParseColor(node.getChild("multiplier")).value_or(Palette::kWhite);
	auto interpolation		= tryParseInterpolation(node.getChild("interpolation")).value_or(Texture::Interpolation::Bilinear);

	return makeImageTexture(path, multiplier, interpolation);
}

std::shared_ptr<Texture> SceneLoader::parseSolidTexture(const NodeHolder& node)
{
	auto color				= tryParseColor(node.getChild("color")).value_or(Palette::kWhite);

	return std::make_shared<SolidTexture>(color);
}

std::shared_ptr<Material> SceneLoader::parseMaterial(const NodeHolder& node)
{
	if (! node)
		return nullptr;

	const auto type = node.getChild("type", true).getValue<std::string>();

	if (type == "Debug")
		return parseDebugMaterial(node);
	else if (type == "Dielectric")
		return parseDielectricMaterial(node);
	else if (type == "Diffuse")
		return parseDiffuseMaterial(node);
	else if (type == "Light")
		return parseLightMaterial(node);
	else if (type == "Reflective")
		return parseReflectiveMaterial(node);
	else
		throw std::runtime_error("Unknown material type '" + type + "' in scene YAML file (" + node.path() + ")");
}

std::shared_ptr<Material> SceneLoader::parseDebugMaterial(const NodeHolder& node)
{
	const auto mode = node.getChild("mode", true).getValue<std::string>();

	if (mode == "Normal")
		return std::make_shared<DebugMaterial>(DebugMaterial::Mode::Normal);
	else if (mode == "UV")
		return std::make_shared<DebugMaterial>(DebugMaterial::Mode::UV);
	else
		throw std::runtime_error("Unknown debug material mode '" + mode + "' in scene YAML file (" + node.path() + ")");
}

std::shared_ptr<Material> SceneLoader::parseDielectricMaterial(const NodeHolder& node)
{
	auto texture			= parseTexture(node.getChild("texture"));
	auto normals			= parseTexture(node.getChild("normals"));
	auto refractionIndex	= tryParseDouble(node.getChild("refractionIndex")).value_or(1.0);

	return std::make_shared<DielectricMaterial>(std::move(texture), std::move(normals), refractionIndex);
}

std::shared_ptr<Material> SceneLoader::parseDiffuseMaterial(const NodeHolder& node)
{
	auto texture			= parseTexture(node.getChild("texture"));
	auto normals			= parseTexture(node.getChild("normals"));

	return std::make_shared<DiffuseMaterial>(std::move(texture), std::move(normals));
}

std::shared_ptr<Material> SceneLoader::parseLightMaterial(const NodeHolder& node)
{
	auto texture			= parseTexture(node.getChild("texture"));
	auto normals			= parseTexture(node.getChild("normals"));

	return std::make_shared<LightMaterial>(std::move(texture), std::move(normals));
}

std::shared_ptr<Material> SceneLoader::parseReflectiveMaterial(const NodeHolder& node)
{
	auto texture			= parseTexture(node.getChild("texture"));
	auto normals			= parseTexture(node.getChild("normals"));
	auto polish				= tryParseDouble(node.getChild("polish")).value_or(1.0);

	return std::make_shared<ReflectiveMaterial>(std::move(texture), std::move(normals), polish);
}

std::optional<Color> SceneLoader::tryParseColor(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	const std::string value = TrimWhitespace(node.getValue<std::string>());

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

	throw std::runtime_error("Unknown color type '" + value + "' in scene YAML file (" + node.path() + ")");
}

std::optional<Vector> SceneLoader::tryParseVector(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	const std::string value = TrimWhitespace(node.getValue<std::string>());

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

	throw std::runtime_error("Unknown vector type '" + value + "' in scene YAML file (" + node.path() + ")");
}

std::optional<Texture::Interpolation> SceneLoader::tryParseInterpolation(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	const std::string value = TrimWhitespace(node.getValue<std::string>());

	static const std::unordered_map<std::string, Texture::Interpolation> kKnownNames
		{
			{ "NearestNeighbor", Texture::Interpolation::NearestNeighbor },
			{ "Bilinear", Texture::Interpolation::Bilinear },
		};
	if (kKnownNames.contains(value))
		return kKnownNames.at(value);

	throw std::runtime_error("Unknown interpolation type '" + value + "' in scene YAML file (" + node.path() + ")");
}

std::optional<double> SceneLoader::tryParseAspectRatio(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	if (const auto doubleValue = node.tryGetValue<double>())
	{
		return *doubleValue;
	}
	else if (const auto stringValue = node.tryGetValue<std::string>())
	{
		std::string value = TrimWhitespace(*stringValue);

		static const std::regex aspectRatioRegex("([^\\,]+)" ":" "([^\\,]+)");
		if (std::smatch matches; std::regex_match(value, matches, aspectRatioRegex))
		{
			auto numerator		= DoubleFromString(matches[1]);
			auto denominator	= DoubleFromString(matches[2]);

			return numerator / denominator;
		}
	}

	throw std::runtime_error("Unknown aspect ratio type in scene YAML file (" + node.path() + ")");
}

std::optional<double> SceneLoader::tryParseDouble(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	if (const auto doubleValue = node.tryGetValue<double>())
		return *doubleValue;
	else if (const auto intValue = node.tryGetValue<int64_t>())
		return static_cast<double>(*intValue);
	else if (const auto stringValue = node.tryGetValue<std::string>())
		return DoubleFromString(*stringValue);

	throw std::runtime_error("Unknown value type in scene YAML file (" + node.path() + ")");
}

std::optional<Camera> SceneLoader::tryParseCamera(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	auto position			= tryParseVector(node.getChild("position")).value_or(StandardVectors::kOrigin);
	auto target				= tryParseVector(node.getChild("target")).value_or(StandardVectors::kUnitZ);
	auto orientation		= tryParseVector(node.getChild("orientation")).value_or(StandardVectors::kUnitY);
	auto aspectRatio		= tryParseAspectRatio(node.getChild("aspectRatio")).value_or(16.0 / 9.0);
	auto verticalFov		= DegreesToRadians(tryParseDouble(node.getChild("verticalFov")).value_or(90));
	auto focusDistance		= tryParseDouble(node.getChild("focusDistance")).value_or(1.0);
	auto aperture			= tryParseDouble(node.getChild("aperture")).value_or(0.0);

	return Camera(position, target, orientation, aspectRatio, verticalFov, focusDistance, aperture);
}

std::optional<Transform> SceneLoader::tryParseTransform(const NodeHolder& node)
{
	if (! node)
		return std::nullopt;

	auto position			= tryParseVector(node.getChild("position")).value_or(StandardVectors::kOrigin);
	auto rotation			= tryParseVector(node.getChild("rotation")).value_or(StandardVectors::kZero);
	auto scale				= tryParseVector(node.getChild("scale")).value_or(StandardVectors::kUnit);

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

	printf("Loading image '%s'\n", path.c_str());

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

	printf("Loading mesh '%s'\n", path.c_str());

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
					.position =
						Vector(
							static_cast<double>(v.Position.X),
							static_cast<double>(v.Position.Y),
							static_cast<double>(-v.Position.Z)
						),

					.normal =
						Vector(
							static_cast<double>(v.Normal.X),
							static_cast<double>(v.Normal.Y),
							static_cast<double>(-v.Normal.Z)
						),

					.texture =
						Vector(
							static_cast<double>(v.TextureCoordinate.X),
							static_cast<double>(v.TextureCoordinate.Y),
							0.0
						)
				}
			);
		}

		for (size_t i = 0; i < mesh.Indices.size(); i += 3)
		{
			triangles.emplace_back(
				Triangle{
					meshStartPos + mesh.Indices[i + 0],
					meshStartPos + mesh.Indices[i + 1],
					meshStartPos + mesh.Indices[i + 2]
				}
			);
		}
	}

	return std::make_shared<Mesh>(std::move(vertices), std::move(triangles));
}
