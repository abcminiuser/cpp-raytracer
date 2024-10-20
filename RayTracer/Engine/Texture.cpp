#include "Engine/Texture.hpp"

#include <algorithm>
#include <cmath>

Texture::Texture(size_t width, size_t height, Interpolation interpolation)
	: m_width(width)
	, m_height(height)
	, m_interpolation(interpolation)
{

}

Color Texture::sample(double u, double v) const
{
	double x = (m_width - 1) * u;
	double y = (m_height - 1) * v;

	switch (m_interpolation)
	{
		case Interpolation::NearestNeighbor:
		{
			return colorAt(size_t(x), size_t(y));
		}

		case Interpolation::Bilinear:
		{
			size_t p = static_cast<size_t>(std::floor(x));
			size_t q = static_cast<size_t>(std::floor(y));

			double dP = x - p;
			double dQ = y - q;

			size_t nextP = (p + 1) % m_width;
			size_t nextQ = (q + 1) % m_height;

			Color result = Palette::kBlack;

			result = result.add(colorAt(p    , q).scale((1 - dP) * (1 - dQ)));
			result = result.add(colorAt(nextP, q).scale(dP * (1 - dQ)));
			result = result.add(colorAt(p    , nextQ).scale((1 - dP) * dQ));
			result = result.add(colorAt(nextP, nextQ).scale(dP * dQ));

			return result;
		}
	}

	return Palette::kBlack;
}
