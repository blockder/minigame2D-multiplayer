#include"Complex.h"

Complex::Complex()
{
	this->i = 0;
	this->real = 0;
}

Complex::Complex(float real, float i)
{
	this->i = i;
	this->real = real;
}
Complex::Complex(glm::vec2 num)
{
	this->i = num.y;
	this->real = num.x;
}

Complex Complex::normalize(Complex complex)
{
	//check if number is 0
	if (glm::length(complex.getVec()) == 0)
		return Complex();

	return Complex(glm::normalize(complex.getVec()));
}

glm::vec2 Complex::getVec()
{
	return glm::vec2(real, i);
}

glm::mat3 Complex::getMatrix()
{
	Complex unitDirection = normalize(*this);
	glm::mat3 matrix(1.0f);
	matrix[0] = glm::vec3((Complex(glm::vec2(1, 0)) * unitDirection).getVec(), 0);
	matrix[1] = glm::vec3((Complex(glm::vec2(0, 1)) * unitDirection).getVec(), 0);
	return matrix;
}

Complex Complex::operator*(Complex other)
{
	Complex res;
	res.real = this->real * other.real - this->i * other.i;
	res.i = other.i * this->real + this->i * other.real;
	return res;
}
