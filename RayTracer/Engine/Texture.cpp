#include "Engine/Texture.hpp"

#include <algorithm>

Color Texture::sample(double u, double v) const
{
	u = std::clamp(u, 0.0, 1.0);
	v = std::clamp(v, 0.0, 1.0);

	return colorAt(u, v);
}
