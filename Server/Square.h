#pragma once
#include<glm/glm.hpp>
#include"Complex.h"
#include<vector>

class Square 
{
public:
	glm::vec2 mid;
	Complex rotation;
	float width;
	float height;

	Square() = default;
	Square(glm::vec2 mid, float width, float height);

	bool touch(Square& other);
	bool inside(Square& other);
	std::vector<glm::vec2> intersectLine(glm::vec2 p, glm::vec2 direction);

	glm::vec2 worldSpace(glm::vec2 point);
private:
	glm::vec2 getNormal(glm::vec2 p1, glm::vec2 p2);

};
