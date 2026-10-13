#pragma once
#include<glm/glm.hpp>

class Camera
{
public:
	glm::vec2 loc;
	float zoom;
	float aspectRatio;

	Camera();
	Camera(glm::vec2 loc, float zoom, int screenWidth, int screenHeight);

	glm::mat4 getMatrix();

	glm::vec2 viewSpace(glm::vec2 point);

	void moveTo(glm::vec2 place);
private:
	float speed;
};