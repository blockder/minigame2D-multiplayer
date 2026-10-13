#include "Camera.h"
#include<glm/gtc/type_ptr.hpp>

Camera::Camera()
{
	this->loc = glm::vec2(0);
	zoom = 1.0f;
	aspectRatio = 1.0f;
	speed = 0;
}
Camera::Camera(glm::vec2 loc, float zoom, int screenWidth, int screenHeight)
{
	this->loc = loc;
	this->zoom = zoom;
	this->aspectRatio = (double)screenHeight / double(screenWidth);
	speed = 0.4;
}

glm::mat4 Camera::getMatrix()
{
	return glm::scale(glm::mat4(1), glm::vec3(zoom * aspectRatio, zoom, 1)) * glm::translate(glm::mat4(1), glm::vec3(-loc, 0));
}

glm::vec2 Camera::viewSpace(glm::vec2 point)
{
	return getMatrix() * glm::vec4(point, 0, 1);
}

void Camera::moveTo(glm::vec2 place)
{
	glm::vec2 direction = place - loc;
	if(glm::length(direction) > 0.1f)
	loc += direction * speed;
}
