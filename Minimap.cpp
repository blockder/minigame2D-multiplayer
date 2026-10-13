#include "Minimap.h"

Minimap::Minimap(glm::vec2 location, float with, float height, int screenWidth, int screenHeight)
{
	square = Square(location, with, height, "textures/blank.png", glm::vec3(1));

	//create FBO
	glCreateFramebuffers(1, &FBO);

	//create texture
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureStorage2D(texture, 1, GL_RGB8, screenWidth, screenHeight);

	//bind texture to framebuffer and square
	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, texture, 0);
	square.getTexture() = texture;

	//check for errors
	auto fboStatus = glCheckNamedFramebufferStatus(FBO, GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "framebuffer error: " << fboStatus << std::endl;
}

void Minimap::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Minimap::unBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
