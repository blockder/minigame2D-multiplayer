#pragma once
#include"Square.h"

class Bullet
{
public:
	int id;
	Square hitbox;
	glm::vec2 velocity;


	Bullet(Square& hitbox, glm::vec2 velocity, int id);

	void update(float time);
};