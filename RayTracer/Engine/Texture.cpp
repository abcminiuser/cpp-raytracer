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
    if (u < 0)
        u -= -1 + std::ceil(u);
	else
		u -= std::floor(u);

	// Wrap V coordinate in both directions to be between [0, 1]
    if (v < 0)
        v -= -1 + std::ceil(v);
	else
		v -= std::floor(v);

	assert(u == std::clamp(u, 0.0, 1.0));
	assert(v == std::clamp(v, 0.0, 1.0));

	// Convert to pixel X/Y texture coordinate.
	double x = (m_width - 1) * u;
	double y = (m_height - 1) * v;

	Color color = Palette::kBlack;

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

			color = color.add(colorAt(p    , q).scale((1 - dP) * (1 - dQ)));
			color = color.add(colorAt(nextP, q).scale(dP * (1 - dQ)));
			color = color.add(colorAt(p    , nextQ).scale((1 - dP) * dQ));
			color = color.add(colorAt(nextP, nextQ).scale(dP * dQ));

			break;
		}
	}

	return color.multiply(m_multiplier);
}