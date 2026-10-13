#pragma once
#include<glm/glm.hpp>
#include<iostream>

class Complex
{
public:
	float i;
	float real;

	Complex();
	Complex(float real, float i);
	Complex(glm::vec2 num);


	glm::vec2 getVec();
	glm::mat3 getMatrix();

	static Complex normalize(Complex complex);

	Complex operator* (Complex other);
};
