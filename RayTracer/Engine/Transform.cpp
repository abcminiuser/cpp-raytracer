#include "Engine/Transform.hpp"

#include <cmath>
#include <stdexcept>

namespace
{
	Matrix<4, 4> TranslateMatrix(const Vector& translation)
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

	Matrix<4, 4> ScaleMatrix(const Vector& scale)
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

	Matrix<4, 4> RotateMatrix(const Vector& rotation)
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

	Vector TransformVector(const Vector& vector, const Matrix<4, 4>& transform, bool allowScaling)
	{
		const Matrix<4, 1> input({
			vector.x(),
			vector.y(),
			vector.z(),
			allowScaling ? 1.0 : 0.0
		});

		const Matrix<4, 1> output = transform * input;

		return Vector(output(0, 0), output(0, 1), output(0, 2));
	}
}

Transform::Transform()
	: Transform(StandardVectors::kZero, StandardVectors::kZero, StandardVectors::kUnit)
{

}

Transform::Transform(const Vector& position, const Vector& rotation, const Vector& scale)
	: m_position(position)
	, m_rotation(rotation)
	, m_scale(scale)
{
	if (scale.length() == 0)
		throw std::runtime_error("Transform created with zero scale.");

	update();
}

void Transform::setPosition(const Vector& position)
{
	if (m_position == position)
		return;

	m_position = position;
	update();
}

void Transform::setRotation(const Vector& rotation)
{
	if (m_rotation == rotation)
		return;

	m_rotation = rotation;
	update();
}

void Transform::setScale(const Vector& scale)
{
	if (m_scale == scale)
		return;

	m_scale = scale;
	update();
}

void Transform::update()
{
	m_forwardTransform	= RotateMatrix(m_rotation.inverted()) * ScaleMatrix(StandardVectors::kUnit / m_scale) * TranslateMatrix(m_position.inverted());
	m_reverseTransform	= (TranslateMatrix(m_position) * RotateMatrix(m_rotation.inverted()) * ScaleMatrix(m_scale)).transposed();
}

Vector Transform::transformPosition(const Vector& vector) const
{
	return TransformVector(vector, m_forwardTransform, true);
}

Vector Transform::transformDirection(const Vector& vector) const
{
	return TransformVector(vector, m_forwardTransform, false).unit();
}

Ray Transform::transformRay(const Ray& ray) const
{
	return Ray(transformPosition(ray.position()), transformDirection(ray.direction()));
}

Vector Transform::untransformPosition(const Vector& vector) const
{
	return TransformVector(vector, m_reverseTransform, true);
}

Vector Transform::untransformDirection(const Vector& vector) const
{
	return TransformVector(vector, m_reverseTransform, false).unit();
}

Ray Transform::untransformRay(const Ray& ray) const
{
	return Ray(untransformPosition(ray.position()), untransformDirection(ray.direction()));
}
