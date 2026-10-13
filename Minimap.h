#pragma once
#include"Square.h"

class Minimap
{
public:
	Square square;

	Minimap(glm::vec2 location, float with, float height, int screenWidth, int screenHeight);

	void bind();
	void unBind();
private:
	GLuint FBO;
	GLuint texture;
};

