#pragma once

#include "Engine/Color.hpp"
#include "Engine/Material.hpp"
#include "Engine/Mesh.hpp"
#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Transform.hpp"
#include "Engine/Vector.hpp"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <fkYAML/node.hpp>

class ImageTexture;

class SceneLoader
{
public:
											SceneLoader() = default;

	Scene									load(const std::string& path);

private:
	void									clearCache();

	std::vector<std::shared_ptr<Object>>	parseObjects(const fkyaml::node& node, const std::string& property);
	std::shared_ptr<Object>					parseBoxObject(const fkyaml::node& node);
	std::shared_ptr<Object>					parseMeshObject(const fkyaml::node& node);
	std::shared_ptr<Object>					parsePlaneObject(const fkyaml::node& node);
	std::shared_ptr<Object>					parseSphereObject(const fkyaml::node& node);

	std::shared_ptr<Texture>				parseTexture(const fkyaml::node& node, const std::string& property);
	std::shared_ptr<Texture>				parseCheckerboardTexture(const fkyaml::node& node);
	std::shared_ptr<Texture>				parseImageTexture(const fkyaml::node& node);
	std::shared_ptr<Texture>				parseSolidTexture(const fkyaml::node& node);

	std::shared_ptr<Material>				parseMaterial(const fkyaml::node& node, const std::string& property);
	std::shared_ptr<Material>				parseDebugMaterial(const fkyaml::node& node);
	std::shared_ptr<Material>				parseDielectricMaterial(const fkyaml::node& node);
	std::shared_ptr<Material>				parseDiffuseMaterial(const fkyaml::node& node);
	std::shared_ptr<Material>				parseLightMaterial(const fkyaml::node& node);
	std::shared_ptr<Material>				parseReflectiveMaterial(const fkyaml::node& node);

	std::optional<Color>					tryParseColor(const fkyaml::node& node, const std::string& property);
	std::optional<Vector>					tryParseVector(const fkyaml::node& node, const std::string& property);
	std::optional<Texture::Interpolation>	tryParseInterpolation(const fkyaml::node& node, const std::string& property);
	std::optional<double>					tryParseAspectRatio(const fkyaml::node& node, const std::string& property);
	std::optional<double>					tryParseDouble(const fkyaml::node& node, const std::string& property);
	std::optional<Camera>					tryParseCamera(const fkyaml::node& node, const std::string& property);
	std::optional<Transform>				tryParseTransform(const fkyaml::node& node, const std::string& property);

	std::shared_ptr<ImageTexture>			makeImageTexture(const std::string& path, const Color& multiplier, Texture::Interpolation interpolation);
	std::shared_ptr<Mesh>					makeObjectMesh(const std::string& path);

private:
	struct Cache
	{
		std::unordered_map<std::string, std::shared_ptr<Mesh>>			meshes;
		std::unordered_map<std::string, std::shared_ptr<ImageTexture>>	imageTextures;
	};

	Cache 	m_cache;
};
