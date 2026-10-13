#include "Player.h"

Player::Player(int clientId, glm::vec3 color)
{
	this->client = clientId;
	this->color = color;
	center = glm::vec2(0);
	sprite = Square(glm::vec2(0), 1.0, 1.0);
	shield = Square(glm::vec2(0), 0.5, 0.5);
	velocity = glm::vec2(0);
	shieldOn = false;
	invulnerable = false;
	lazerOn = false;
	lazer = glm::vec2(0);
	lazerColor = color;
	invulnerableTimer = 0;
}

void Player::hitPlayer(Server& server)
{
	if (invulnerable)
		return;

	invulnerable = true;
	invulnerableTimer = 20;

	//send to clients
	Message output2("player");
	output2.addSimple(client);
	output2.add("invulnerable");
	output2.addSimple(invulnerable);
	server.addEvent(output2, -1, ALL, false);
}

void Player::updateVulnerbale(Server& server)
{
	if (!invulnerable)
		return;
	if (invulnerableTimer > 0)
	{
		invulnerableTimer--;
		return;
	}
	invulnerable = false;
	Message output2("player");
	output2.addSimple(client);
	output2.add("invulnerable");
	output2.addSimple(invulnerable);
	server.addEvent(output2, -1, ALL, false);
}
void Player::knockback(Server& server, glm::vec2 knockback)
{
	if (invulnerable)
		return;
	knockback.y = 0;
	knockback = glm::normalize(knockback);
	knockback.y = 0.5f;

	Message output("player");
	output.addSimple(client);
	output.add("knockback");
	output.addSimple(knockback);
	server.addUpdate(output, client, ONE);
}


