#pragma once

#include "Engine/Color.hpp"

class Texture
{
public:
	enum class Interpolation
	{
		NearestNeighbor,
		Bilinear
	};

						Texture(size_t width, size_t height, const Color& multiplier, Interpolation interpolation);
	virtual				~Texture() = default;

	Color				sample(double u, double v) const;

protected:
	virtual Color		colorAt(size_t x, size_t y) const = 0;

protected:
	const size_t		m_width;
	const size_t		m_height;

private:
	Color				m_multiplier;
	Interpolation		m_interpolation = Interpolation::Bilinear;
};
