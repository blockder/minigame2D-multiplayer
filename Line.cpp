#include "Line.h"
#include<glm/gtc/type_ptr.hpp>

Line::Line(glm::vec3 color) : Mesh()
{
	p1 = glm::vec2(0);
	p2 = glm::vec2(0);
	this->color = color;

	createTexture("textures/blank.png");

	//create vertices
	vertices.push_back(Vertex(glm::vec3(p1, 0), glm::vec3(color), glm::vec2(0)));
	vertices.push_back(Vertex(glm::vec3(p2, 0), glm::vec3(color), glm::vec2(0)));

	//create indices
	indices = { 0, 1 };

	loadVBO();
}

void Line::draw(Shader& shaderProgram, Camera& camera)
{
	shaderProgram.Active();

	//bind VAO and texture
	glBindVertexArray(VAO);
	glBindTextureUnit(0, Texture);

	//send pos to shader
	glUniform3fv(glGetUniformLocation(shaderProgram.id, "movement"), 1, glm::value_ptr(glm::vec3(pos, 0)));

	//send rotation to shader
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation.getMatrix()));

	//send camera matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getMatrix()));

	//draw call
	glLineWidth(3.0f);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

	//unbind VAO and texture
	glBindVertexArray(0);
	glBindTextureUnit(0, 0);
}
void Line::updateLine(glm::vec2 p1, glm::vec2 p2)
{
	this->p1 = p1;
	this->p2 = p2;
	vertices[0].position = glm::vec3(p1, 0);
	vertices[1].position = glm::vec3(p2, 0);

	loadVBO();
}
void Line::setColor(glm::vec3 color)
{
	this->color = color;
	vertices[0].color = color;
	vertices[1].color = color;
	loadVBO();
}
