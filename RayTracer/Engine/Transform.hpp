#pragma once

#include "Engine/Matrix.hpp"
#include "Engine/Vector.hpp"

class Ray;

class Transform
{
public:
					Transform();
					Transform(const Vector& position, const Vector& rotation, const Vector& scale);

	void			setPosition(const Vector& position);
	void			setRotation(const Vector& position);
	void			setScale(const Vector& position);

	Vector			transformPosition(const Vector& vector) const;
	Vector			transformDirection(const Vector& vector) const;
	Ray				transformRay(const Ray& ray) const;

	Vector			untransformPosition(const Vector& vector) const;
	Vector			untransformDirection(const Vector& vector) const;
	Ray				untransformRay(const Ray& ray) const;

private:
	void			update();

private:
	Vector			m_position;
	Vector			m_rotation;
	Vector			m_scale;

	Matrix<4, 4>	m_forwardTransform;
	Matrix<4, 4>	m_reverseTransform;
};
