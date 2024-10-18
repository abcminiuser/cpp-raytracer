#include "DebugMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

DebugMaterial::DebugMaterial(Type type)
	: Material(nullptr)
	, m_type(type)
{

}

Color DebugMaterial::emit(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv)
{
	switch (m_type)
	{
		case Type::Normal:
			return Color(normal.x(), normal.y(), normal.z());

		case Type::UV:
			return Color(uv.x(), uv.y(), 0);
	}

	return Palette::kMagenta;
}
