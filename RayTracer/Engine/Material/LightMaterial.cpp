#include "LightMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

LightMaterial::LightMaterial(std::shared_ptr<Texture> texture)
	: Material(std::move(texture))
{

}

Color LightMaterial::emit(const Vector& uv)
{
	return m_texture->colorAt(uv.x(), uv.y());
}
