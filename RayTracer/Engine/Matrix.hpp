#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>

template <size_t ROWS, size_t COLS>
struct Matrix
{
public:
	constexpr								Matrix() = default;

	constexpr								Matrix(std::convertible_to<double> auto&&... values)
		: m_elements({ std::forward<decltype(values)>(values)... })
	{

	}

	template <size_t OTHERCOLS>
	constexpr Matrix<ROWS, OTHERCOLS>		operator*(const Matrix<ROWS, OTHERCOLS>& other) const
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

private:
	std::array<double, ROWS * COLS>			m_elements = {};
};
