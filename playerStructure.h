#pragma once
#include"Square.h"
#include<GLFW/glfw3.h>
#include"Bullet.h"
#include"Camera.h"
#include"Line.h"
#include<queue>
enum class Gun
{
	normal,
	lazer,
	shotgun,
	minigun,
};

struct Stats
{
	int health = 10;
	bool invulnerable = false;
	bool shieldOn = false;
	int shieldCoolDownTimer = 0;
	int shieldCoolDown = 100;
	int shieldUpTimer = 0;
};

struct Visual
{
	Square sprite;
	Square shield;
	Square coolDownBar;
	bool showBar;
	glm::vec3 color;
	Visual() = default;
	Visual(Square& sprite, Square& shield, Square& coolDownBar, glm::vec3 color)
	{
		this->sprite = sprite;
		this->shield = shield;
		this->coolDownBar = coolDownBar;
		this->color = color;
		showBar = false;
	}
};

struct Guns
{
	Gun gun = Gun::normal;
	bool ableToFire = false;
	int lazerDelay = 0;
	int ammo = 0;
	int shootDelay = 0;
	int minigunCooldown = 0;
	bool lazerOn = false;
	Line lazer;

	Guns() = default;
	Guns(glm::vec3 lazerColor)
	{
		this->lazer = Line(lazerColor);
	}
};

struct Movement
{
	glm::vec2 center = glm::vec2(0);
	glm::vec2 velocity = glm::vec2(0);
	float speed = 25.0f;
	bool ableToJump = false;
	int jumpsLeft = 0;
	bool switchTarget = true;
	glm::vec2 target = glm::vec2(0);
	std::queue<glm::vec2> targetQueue;
	double lastBlockStanding = -50;
	double fallHeight = 0;
};