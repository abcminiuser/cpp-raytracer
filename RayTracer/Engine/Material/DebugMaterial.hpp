#pragma once

#include "Engine/Material.hpp"
#include "Engine/Ray.hpp"

#include <memory>

class Texture;

class DebugMaterial final
	: public Material
{
public:
	enum class Type
	{
		Normal,
		UV
	};

							DebugMaterial(Type type);
							~DebugMaterial() = default;

// Material i/f:
public:
	Color					emit(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv) override;

private:
	Type					m_type;
};
