#pragma once
#include"Square.h"
#include"Server.h"


class Player
{
public:
	int client;
	glm::vec3 color;
	Square sprite;
	Square shield;
	glm::vec2 center;
	glm::vec2 velocity;
	bool lazerOn;
	glm::vec2 lazer;
	glm::vec3 lazerColor;

	bool shieldOn;
	bool invulnerable;

	Player(int clientId, glm::vec3 color);
	void hitPlayer(Server& server);
	void knockback(Server& server, glm::vec2 knockback);
	void updateVulnerbale(Server& server);
private:
	int invulnerableTimer;
};