#include "DebugMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

DebugMaterial::DebugMaterial(Mode mode)
	: Material(nullptr, nullptr)
	, m_mode(mode)
{

}

Color DebugMaterial::emit(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv)
{
	switch (m_mode)
	{
		case Mode::Normal:
			return Color(normal.x(), normal.y(), normal.z());

		case Mode::UV:
			return Color(uv.x(), uv.y(), 0);
	}

	return Palette::kMagenta;
}
