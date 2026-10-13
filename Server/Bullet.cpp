#include"Bullet.h"

Bullet::Bullet(Square& hitbox, glm::vec2 velocity, int id, int playerId, bool bounce)
{
	this->id = id;
	this->playerId = playerId;
	this->hitbox = hitbox;
	this->velocity = velocity;
	this->bounce = bounce;
	life = 2;
}

void Bullet::update(float tickDuration)
{
	hitbox.mid += velocity * tickDuration;
	if (life == 0)
		bounce = false;

}


