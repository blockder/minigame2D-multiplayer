#pragma once
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>
#include"Shader.h"
#include"Complex.h"
#include"Camera.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 UV;
	Vertex(glm::vec3 position, glm::vec3 color, glm::vec2 texCoord) : position(position), color(color)
		, UV(texCoord){ }
};
class Mesh
{
public:
	glm::vec2 pos;
	Complex rotation;
	
	Mesh();

	virtual void draw(Shader& shaderProgram);
	virtual void draw(Shader& shaderProgram, Camera& camera);

	glm::vec2 worlsSpace(glm::vec2 point);

	GLuint& getTexture();
	
protected:
	std::vector <Vertex> vertices;
	std::vector <unsigned int> indices;

	void loadVBO();
	void createTexture(const char* source);

	GLuint VAO;
	GLuint EBO;
	GLuint VBO;
	GLuint Texture;

};