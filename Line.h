#pragma once
#include"Mesh.h"

class Line : public Mesh
{
public:
	glm::vec2 p1;
	glm::vec2 p2;
	glm::vec3 color;

	Line() = default;
	Line(glm::vec3 color);
	
	void draw(Shader& shaderProgram, Camera& camera);

	void updateLine(glm::vec2 p1, glm::vec2 p2);
	void setColor(glm::vec3 color);


};
