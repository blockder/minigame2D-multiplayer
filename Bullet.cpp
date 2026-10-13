#include"Bullet.h"

Bullet::Bullet(Square& hitbox, glm::vec2 velocity, int id)
{
	this->id = id;
	this->hitbox = hitbox;
	this->velocity = velocity;
}

void Bullet::update(float time)
{
	hitbox.pos += velocity * time;
}