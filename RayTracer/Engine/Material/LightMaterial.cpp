#include "LightMaterial.hpp"

#include "Engine/Ray.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vector.hpp"

LightMaterial::LightMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> normals)
	: Material(std::move(texture), std::move(normals))
{

}

Color LightMaterial::emit(const Vector& incident, const Vector& position, const Vector& normal, const Vector& uv)
{
	return m_texture->sample(uv.x(), uv.y());
}
