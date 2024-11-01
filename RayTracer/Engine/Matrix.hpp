#pragma once

#include "Engine/Vector.hpp"
#include "Engine/Ray.hpp"

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

	constexpr Matrix<COLS, ROWS>			transposed() const
	{
		Matrix<COLS, ROWS> output;

		for (size_t i = 0; i < ROWS; i++)
		{
			for (size_t k = 0; k < COLS; k++)
				output(k, i) = operator()(i, k);
		}

		return output;
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
	std::array<double, ROWS * COLS>			m_elements;
};

namespace MatrixUtils
{
	static inline Matrix<4, 4>				TranslateMatrix(const Vector& translation)
	{
		const auto& x = translation.x();
		const auto& y = translation.y();
		const auto& z = translation.z();

		return Matrix<4, 4>
			({
				1.0, 0.0, 0.0,   x,
				0.0, 1.0, 0.0,   y,
				0.0, 0.0, 1.0,   z,
				0.0, 0.0, 0.0, 1.0
			});
	}

	static inline Matrix<4, 4>				ScaleMatrix(const Vector& scale)
	{
		const auto& x = scale.x();
		const auto& y = scale.y();
		const auto& z = scale.z();

		return Matrix<4, 4>
			({
				  x, 0.0, 0.0, 0.0,
				0.0,   y, 0.0, 0.0,
				0.0, 0.0,   z, 0.0,
				0.0, 0.0, 0.0, 1.0
			});
	}

	static inline Matrix<4, 4>				RotateMatrix(const Vector& rotation)
	{
		const auto alpha = rotation.x();
		const auto cosAlpha = std::cos(alpha);
		const auto sinAlpha = std::sin(alpha);

		const auto beta = rotation.y();
		const auto cosBeta = std::cos(beta);
		const auto sinBeta = std::sin(beta);

		const auto gamma = rotation.z();
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

		return Matrix<4, 4>
			({
				m00, m01, m02, 0.0,
				m10, m11, m12, 0.0,
				m20, m21, m22, 0.0,
				0.0, 0.0, 0.0, 1.0
			});
	}

	static inline Vector 					Transform(const Vector& vector, const Matrix<4, 4>& transform, bool isDirection)
	{
		const Matrix<4, 1> input({
			vector.x(),
			vector.y(),
			vector.z(),
			isDirection ? 0.0 : 1.0
		});

		const Matrix<4, 1> output = transform * input;

		return Vector(output(0, 0), output(0, 1), output(0, 2));
	}

	static inline Ray 						Transform(const Ray& ray, const Matrix<4, 4>& transform)
	{
		return Ray(Transform(ray.position(), transform, false), Transform(ray.direction(), transform, true).unit());
	}
}
