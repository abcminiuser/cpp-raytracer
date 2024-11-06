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

class ImageTexture;

class SceneLoader
{
public:
											SceneLoader() = default;

	Scene									load(const std::string& path);

private:
	struct NodeHolder;

	Scene									parseScene(const NodeHolder& node);
	std::vector<std::shared_ptr<Object>>	parseObjects(const NodeHolder& node);

	std::shared_ptr<Object>					parseObject(const NodeHolder& node);
	std::shared_ptr<Object>					parseBoxObject(const NodeHolder& node);
	std::shared_ptr<Object>					parseMeshObject(const NodeHolder& node);
	std::shared_ptr<Object>					parsePlaneObject(const NodeHolder& node);
	std::shared_ptr<Object>					parseSphereObject(const NodeHolder& node);

	std::shared_ptr<Texture>				parseTexture(const NodeHolder& node);
	std::shared_ptr<Texture>				parseCheckerboardTexture(const NodeHolder& node);
	std::shared_ptr<Texture>				parseImageTexture(const NodeHolder& node);
	std::shared_ptr<Texture>				parseSolidTexture(const NodeHolder& node);

	std::shared_ptr<Material>				parseMaterial(const NodeHolder& node);
	std::shared_ptr<Material>				parseDebugMaterial(const NodeHolder& node);
	std::shared_ptr<Material>				parseDielectricMaterial(const NodeHolder& node);
	std::shared_ptr<Material>				parseDiffuseMaterial(const NodeHolder& node);
	std::shared_ptr<Material>				parseLightMaterial(const NodeHolder& node);
	std::shared_ptr<Material>				parseReflectiveMaterial(const NodeHolder& node);

	std::optional<Color>					tryParseColor(const NodeHolder& node);
	std::optional<Vector>					tryParseVector(const NodeHolder& node);
	std::optional<Texture::Interpolation>	tryParseInterpolation(const NodeHolder& node);
	std::optional<double>					tryParseAspectRatio(const NodeHolder& node);
	std::optional<double>					tryParseDouble(const NodeHolder& node);
	std::optional<Camera>					tryParseCamera(const NodeHolder& node);
	std::optional<Transform>				tryParseTransform(const NodeHolder& node);

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
