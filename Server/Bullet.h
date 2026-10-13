#pragma once
#include"Square.h"
#include<vector>

class Bullet
{
public:
	int id;
	int playerId;
	Square hitbox;
	glm::vec2 velocity;
	bool bounce;
	int life;

	Bullet() = default;
	Bullet(Square& hitbox, glm::vec2 velocity, int id, int playerId, bool bounce);

	void update(float tickDuration);
};