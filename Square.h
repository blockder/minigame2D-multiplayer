#pragma once
#include"Mesh.h"

class Square : public Mesh
{
public:
	float width;
	float height;

	Square() = default;
	Square(glm::vec2 pos, float width, float height, const char* texture, glm::vec3 color);

	bool touch(Square& other);
	std::vector<glm::vec2> intersectLine(glm::vec2 p, glm::vec2 direction);
	void setColor(glm::vec3 color);
	glm::vec3 getColor();
	void loadPoints();

};
