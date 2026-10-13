#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb/stb_image.h"
#include<glm/gtc/type_ptr.hpp>

#include<iostream>

Mesh::Mesh()
{
	pos = glm::vec2(0);
	rotation = Complex(1,0);
	//create VAO, EBO and VBO
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	//connect VBO and EBO to VAO
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex)); //connects VBO to buffer slot 0 in VAO
	glVertexArrayElementBuffer(VAO, EBO);

	//set layouts
	glEnableVertexArrayAttrib(VAO, 0);													//positions
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, (GLuint)0);

	glEnableVertexArrayAttrib(VAO, 1);													//colors
	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, (GLuint)3 * sizeof(float));

	glEnableVertexArrayAttrib(VAO, 2);													//texCoord
	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, (GLuint)6 * sizeof(float));

	//assign layouts to buffer 0 
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribBinding(VAO, 2, 0);

}
void Mesh::draw(Shader& shaderProgram)
{
	Camera defaultCam;
	draw(shaderProgram, defaultCam);
}
void Mesh::draw(Shader& shaderProgram, Camera& camera)
{
	shaderProgram.Active();

	//bind VAO and texture
	glBindVertexArray(VAO);
	int textureUnit = 0;
	glBindTextureUnit(textureUnit, Texture);

	//send texture to shader
	glUniform1i(glGetUniformLocation(shaderProgram.id, "texUnit"), textureUnit);

	//send pos to shader
	glUniform3fv(glGetUniformLocation(shaderProgram.id, "movement"), 1, glm::value_ptr(glm::vec3(pos, 0)));

	//send rotation to shader
	glUniformMatrix3fv(glGetUniformLocation(shaderProgram.id, "rotation"), 1, GL_FALSE, glm::value_ptr(rotation.getMatrix()));

	//send camera matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(camera.getMatrix()));

	//draw call
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


	//unbind VAO and texture
	glBindVertexArray(0);
	glBindTextureUnit(textureUnit, 0);
}
void Mesh::loadVBO()
{
	//put data in VBO 
	glNamedBufferData(VBO, sizeof(Vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

	//set EBO
	glNamedBufferData(EBO, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}
glm::vec2 Mesh::worlsSpace(glm::vec2 point)
{
	return (Complex(point) * rotation).getVec() + pos;
}

GLuint& Mesh::getTexture()
{
	return Texture;
}

void Mesh::createTexture(const char* source)
{
	int width, height, numColch;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(source, &width, &height, &numColch, 0);
	
	if (!bytes)
		throw std::runtime_error("Failed to load texture");

	glCreateTextures(GL_TEXTURE_2D, 1, &Texture);
	//set parameters
	glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	int mipmapLevel = 5;
	//put image into texture object
	if (numColch == 4)
	{
		glTextureStorage2D(Texture, mipmapLevel, GL_RGBA8, width, height);
		glTextureSubImage2D(Texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}
	else if (numColch == 3)
	{
		glTextureStorage2D(Texture, mipmapLevel, GL_RGB8, width, height);
		glTextureSubImage2D(Texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	else if (numColch == 1)
	{
		glTextureStorage2D(Texture, mipmapLevel, GL_R8, width, height);
		glTextureSubImage2D(Texture, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, bytes);
	}
	else throw std::invalid_argument("automatic texture type recognition falied");

	glGenerateTextureMipmap(Texture);

	stbi_image_free(bytes);
}
