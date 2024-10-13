#pragma once

#include "Engine/Vector.hpp"

#include <array>
#include <cmath>
#include <stdint.h>

template <size_t ROWS, size_t COLS>
class Matrix
{
public:
	template <size_t OTHERCOLS>
	constexpr Matrix<ROWS, OTHERCOLS>	multiply(const Matrix<ROWS, OTHERCOLS>& other) const
	{
		Matrix<ROWS, OTHERCOLS> result;

		for (size_t i = 0; i < ROWS; i++)
		{
			for (size_t j = 0; j < OTHERCOLS; j++)
			{
				for (size_t k = 0; k < COLS; k++)
					result(i, j) += operator()(i, k) * other(k, j);
			}
		}

		return result;
	}

	constexpr double&					operator()(size_t row, size_t col)
	{
		return m_elements[row][col];
	}

	constexpr const double&				operator()(size_t row, size_t col) const
	{
		return m_elements[row][col];
	}

private:
	std::array<std::array<double, COLS>, ROWS> m_elements = {};
};

namespace MatrixUtils
{
	static inline Matrix<3, 3>			RotationMatrix(const Vector& rotation)
	{
		const auto& alpha = rotation.x();
		const auto cosAlpha = std::cos(alpha);
		const auto sinAlpha = std::sin(alpha);

		const auto& beta = rotation.y();
		const auto cosBeta = std::cos(beta);
		const auto sinBeta = std::sin(beta);

		const auto& gamma = rotation.z();
		const auto cosGamma = std::cos(gamma);
		const auto sinGamma = std::sin(gamma);

		Matrix<3, 3> rotationMatrix;
		rotationMatrix(0, 0) = cosAlpha * cosBeta;
		rotationMatrix(0, 1) = (cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma);
		rotationMatrix(0, 2) = (cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma);
		rotationMatrix(1, 0) = sinAlpha * cosBeta;
		rotationMatrix(1, 1) = (sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma);
		rotationMatrix(1, 2) = (sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma);
		rotationMatrix(2, 0) = -sinBeta;
		rotationMatrix(2, 1) = cosBeta * sinGamma;
		rotationMatrix(2, 2) = cosBeta * cosGamma;

		return rotationMatrix;
	}
}
