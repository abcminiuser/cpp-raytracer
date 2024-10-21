#pragma once

#include "Engine/Vector.hpp"

#include <array>
#include <cmath>
#include <concepts>
#include <stdint.h>

template <size_t ROWS, size_t COLS>
class Matrix
{
public:
	constexpr								Matrix()
		: m_elements({})
	{

	}

	constexpr								Matrix(std::convertible_to<double> auto&&... values)
		: m_elements({ std::forward<decltype(values)>(values)... })
	{

	}

	constexpr								Matrix(const Vector& vector)
		requires (ROWS == 3 && COLS == 1)
		: m_elements({ vector.x(), vector.y(), vector.z() })
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
			for (size_t k = 0; k < COLS; k++)
			{
				for (size_t j = 0; j < OTHERCOLS; j++)
					result(i, j) += operator()(i, k) * other(k, j);
			}
		}

		return result;
	}

	constexpr inline double&				operator()(size_t row, size_t col)
	{
		return m_elements[row * COLS + col];
	}

	constexpr inline const double&			operator()(size_t row, size_t col) const
	{
		return m_elements[row * COLS + col];
	}

	constexpr Vector						toVector() const
		requires (ROWS == 3 && COLS == 1)
	{
		return Vector(m_elements[0], m_elements[1], m_elements[2]);
	}

private:
	std::array<double, ROWS * COLS>			m_elements;
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

		const auto m00 = cosAlpha * cosBeta;
		const auto m01 = (cosAlpha * sinBeta * sinGamma) - (sinAlpha * cosGamma);
		const auto m02 = (cosAlpha * sinBeta * cosGamma) + (sinAlpha * sinGamma);

		const auto m10 = sinAlpha * cosBeta;
		const auto m11 = (sinAlpha * sinBeta * sinGamma) + (cosAlpha * cosGamma);
		const auto m12 = (sinAlpha * sinBeta * cosGamma) - (cosAlpha * sinGamma);

		const auto m20 = -sinBeta;
		const auto m21 = cosBeta * sinGamma;
		const auto m22 = cosBeta * cosGamma;

		return Matrix<3, 3>({
			m00, m01, m02,
			m10, m11, m12,
			m20, m21, m22
		});
	}
}
