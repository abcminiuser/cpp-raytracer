#pragma once

#include "Engine/Matrix.hpp"
#include "Engine/Vector.hpp"

class BoundingBox;

class Transform
{
public:
					Transform();

	const Vector&	position() const	{ return m_position; }
	const Vector&	rotation() const	{ return m_rotation; }
	const Vector&	scale() const		{ return m_scale; }

	void			setPosition(const Vector& position);
	void			setRotation(const Vector& roation);
	void			setScale(const Vector& scale);

	Vector			transformPosition(const Vector& vector) const;
	Vector			transformDirection(const Vector& vector) const;
	BoundingBox		transformBoundingBox(const BoundingBox& boundingBox) const;

	Vector			untransformPosition(const Vector& vector) const;
	Vector			untransformDirection(const Vector& vector) const;
	BoundingBox		untransformBoundingBox(const BoundingBox& boundingBox) const;

private:
	void			update();

private:
	Vector			m_position = StandardVectors::kZero;
	Vector			m_rotation = StandardVectors::kZero;
	Vector			m_scale = StandardVectors::kUnit;

	Matrix<4, 4>	m_forwardTransform;
	Matrix<4, 4>	m_reverseTransform;
};
