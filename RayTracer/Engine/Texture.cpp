#include "Engine/Texture.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

Texture::Texture(size_t width, size_t height, const Color& multiplier, Interpolation interpolation)
	: m_width(width)
	, m_height(height)
	, m_multiplier(multiplier)
	, m_interpolation(interpolation)
{

}

Color Texture::sample(double u, double v) const
{
	// Wrap U coordinate in both directions to be between [0, 1]
	u = std::fmod(u, 1.0);
	if (u < 0)
		u += 1.0;

	v = std::fmod(v, 1.0);
	if (v < 0)
		v += 1.0;

	assert(u == std::clamp(u, 0.0, 1.0));
	assert(v == std::clamp(v, 0.0, 1.0));

	// Convert to pixel X/Y texture coordinate.
	double x = (m_width - 1) * u;
	double y = (m_height - 1) * v;

	Color color;

	switch (m_interpolation)
	{
		case Interpolation::NearestNeighbor:
		{
			size_t p = static_cast<size_t>(x);
			size_t q = static_cast<size_t>(y);

			color = colorAt(p, q);
			break;
		}

		case Interpolation::Bilinear:
		{
			size_t p = static_cast<size_t>(x);
			size_t q = static_cast<size_t>(y);

			double dP = x - p;
			double dQ = y - q;

			size_t nextP = (p + 1) % m_width;
			size_t nextQ = (q + 1) % m_height;

			color += colorAt(p    , q) * (1 - dP) * (1 - dQ);
			color += colorAt(nextP, q) * dP * (1 - dQ);
			color += colorAt(p    , nextQ) * (1 - dP) * dQ;
			color += colorAt(nextP, nextQ) * dP * dQ;
			break;
		}
	}

	return color * m_multiplier;
}
