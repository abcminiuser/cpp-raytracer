#pragma once

#include "Engine/Vector.hpp"

#include <array>
#include <cmath>
#include <stdint.h>

template <size_t ROWS, size_t COLS>
class Matrix
{
public:
	using ElementStorage = std::array<std::array<double, COLS>, ROWS>;

	constexpr								Matrix() = default;

	constexpr								Matrix(const ElementStorage& values)
		: m_elements(values)
	{

	}

	constexpr								Matrix(const Vector& vector)
		requires (ROWS == 3 && COLS == 1)
		: Matrix(std::array
			{
				std::array{ vector.x() },
				std::array{ vector.y() },
				std::array{ vector.z() }
			})
	{

	}

	constexpr Matrix<3, 1>					multiply(const Vector& vector) const
		requires (ROWS == 3)
	{
		return multiply(Matrix<3, 1>(vector));
	}

	template <size_t OTHERCOLS>
	constexpr Matrix<ROWS, OTHERCOLS>		multiply(const Matrix<ROWS, OTHERCOLS>& other) const
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

	constexpr double&						operator()(size_t row, size_t col)
	{
		return m_elements[row][col];
	}

	constexpr const double&					operator()(size_t row, size_t col) const
	{
		return m_elements[row][col];
	}

	constexpr Vector						toVector() const
		requires (ROWS == 3 && COLS == 1)
	{
		return Vector(m_elements[0][0], m_elements[1][0], m_elements[2][0]);
	}

private:
	ElementStorage							m_elements = {};
};

namespace MatrixUtils
{
	static inline Matrix<3, 3>				RotationMatrix(const Vector& rotation)
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

		return Matrix<3, 3>(
			std::array
			{
				std::array{
					cosAlpha * cosBeta,
					(cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma),
					(cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma)
				},
				std::array{
					sinAlpha * cosBeta,
					(sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma),
					(sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma)
				},
				std::array{
					-sinBeta,
					cosBeta * sinGamma,
					cosBeta * cosGamma
				}
			});
	}
}
