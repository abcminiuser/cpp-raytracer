#pragma once

#include <stdint.h>
#include <vector>

class Matrix
{
public:
	Matrix(size_t rows, size_t cols)
		: m_rows(rows)
		, m_cols(cols)
		, m_elements(rows * cols)
	{

	}

	size_t	rows() const
	{
		return m_rows;
	}

	size_t	cols() const
	{
		return m_cols;
	}

	Matrix	multiply(const Matrix& other) const
	{
		Matrix result(m_rows, other.m_cols);

		for (size_t i = 0; i < m_rows; i++)
		{
			for (size_t j = 0; j < result.m_cols; j++)
			{
				for (size_t k = 0; k < m_cols; k++)
					result(i, j) += operator()(i, k) * other(j, k);
			}
		}

		return result;
	}

	double& operator()(size_t row, size_t col)
	{
		return m_elements[row * m_cols + col];
	}

	const double& operator()(size_t row, size_t col) const
	{
		return m_elements[row * m_cols + col];
	}

private:
	size_t				m_rows;
	size_t				m_cols;
	std::vector<double> m_elements;
};
