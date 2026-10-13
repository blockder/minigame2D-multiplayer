#include "player.h"
#include"Collision.h"
#include"Client.h"
#include<thread>
#include<chrono>

const float gravity = -130;
Player::Player(Square& sprite, Square& shield, Square& coolDownBar, glm::vec3 color)
{
	id = 0;
	this->visual = Visual(sprite, shield, coolDownBar, color);
	this->guns = Guns(color);
}

void Player::interpolate(float time)
{
	if (glm::distance(movement.center, movement.target) < 0.1f)
	{
		movement.velocity = glm::vec2(0);
		return;
	}
	movement.center += movement.velocity * time;
	visual.sprite.pos = movement.center;
}

void Player::addTarget(glm::vec2 newTarget)
{
	movement.targetQueue.push(newTarget);
	const int maxTarget = 2;
	if (movement.targetQueue.size() > maxTarget)
	{
		movement.targetQueue.pop();
	}
	if (newTarget == movement.center)
		movement.switchTarget = false;
	else movement.switchTarget = true;

}
void Player::nextTarget(float time)
{
	//don't switch
	if (movement.targetQueue.size() == 0 || !movement.switchTarget)
	{
		movement.velocity = glm::vec2(0);
		return;
	}
	movement.target = movement.targetQueue.front();
	movement.targetQueue.pop();
	movement.velocity = (movement.target - movement.center) / time;
}


void Player::update(GLFWwindow* window, Camera& camera, float tickDuration, std::vector<Square>& map)
{
	//keyboard input
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		movement.center += glm::vec2(1, 0) * movement.speed * tickDuration;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		movement.center -= glm::vec2(1, 0) * movement.speed * tickDuration;
	}

	//integrate 
	movement.velocity += glm::vec2(0, 1) * gravity * tickDuration;
	movement.center += movement.velocity * tickDuration;

	//update sprite pos
	visual.sprite.pos = movement.center;

	//collision
	std::vector<Contact> contacts = Collision::CollisionDetection(visual.sprite, map);
	Collision::resolveContacts(contacts, *this);

	//jump
	jumpMechanics(window, contacts);

	//shield
	updateShield(window, camera);
	updateCoolDownBar();
}

void Player::shoot(GLFWwindow* window, Camera& camera, Client& client, std::vector<Square>& map, std::vector<Player>& players)
{
	switch (guns.gun)
	{
		case Gun::normal: 
		{
			fireBullet(window, camera, client);
			break;
		}
		case Gun::lazer:
		{
			shootLazer(window, camera, client, map, players);
			break;
		}
		case Gun::shotgun:
		{
			shotgun(window, camera, client);
			break;
		}
		case Gun::minigun:
		{
			minigun(window, camera, client);
			break;
		}
	}
}

void Player::sendShield(Client& client)
{
	//send shield
	Message outputShield("player");
	outputShield.add("shield");
	outputShield.addSimple(stats.shieldOn);
	outputShield.addSimple(visual.shield.pos);
	outputShield.addSimple(visual.shield.rotation);
	outputShield.addSimple(visual.shield.width);
	outputShield.addSimple(visual.shield.height);
	client.addUpdate(outputShield);

	//send shield cooldown
	Message output("player");
	output.add("shieldCooDownTimer");
	output.addSimple(stats.shieldCoolDownTimer);
	client.addUpdate(output);
}

void Player::sendMovement(Client& client)
{
	Message output("player");
	output.add("movement");
	output.addSimple(movement.center);
	client.addUpdate(output);
}






glm::vec2 Player::getMousePos(GLFWwindow* window)
{
	//get mouse pos in world coordinates
	double mouseX, mouseY;
	int screenWidth, screenHeight;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	glfwGetWindowSize(window, &screenWidth, &screenHeight);
	double aspectRation = (double)screenHeight / (double)screenWidth;

	//step 1 normalize window coordinate
	glm::vec2 mousePos(mouseX / screenWidth, mouseY / screenHeight);

	//step 2 reverse y coordinate
	mousePos.y *= -1;

	//step 3 add offset
	glm::vec2 offSet = glm::vec2(-0.5f, 0.5f);
	mousePos += offSet;

	//step 4 multiply by 2
	mousePos *= 2.0f;
	
	//step 5 multipy by aspectRatio
	mousePos.x /= aspectRation;

	return mousePos;
}
void Player::jumpMechanics(GLFWwindow* window, std::vector<Contact>& contacts)
{	
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		movement.ableToJump = true;
	if (movement.ableToJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && movement.jumpsLeft > 0)
	{
		movement.velocity.y = 42.0f;
		movement.ableToJump = false;
		movement.jumpsLeft--;
	}

}
void Player::updateShield(GLFWwindow* window, Camera& camera)
{
	if (stats.shieldCoolDownTimer > 0)
		return;

	//return if not right click
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		stats.shieldOn = false;
		return;
	}

	stats.shieldOn = true;
	guns.ableToFire = false;
	glm::vec2 mousePos = getMousePos(window);
	
	Complex direction = Complex::normalize(mousePos - camera.viewSpace(movement.center));

	//update shield position and rotation
	visual.shield.pos = movement.center + direction.getVec()* 2.3f;
	visual.shield.rotation = direction;
}
void Player::updateCoolDownBar()
{
	int coolDown = stats.shieldCoolDownTimer;
	if (coolDown == 0)
	{
		visual.showBar = false;
		return;
	}
	visual.showBar = true;
	Square& bar = visual.coolDownBar;
	float barLength = (float)coolDown / stats.shieldCoolDown;
	bar.width = barLength * visual.sprite.width;
	bar.pos.x = visual.sprite.pos.x - visual.sprite.width + (barLength * visual.sprite.width);
	bar.pos.y = visual.sprite.pos.y - visual.sprite.height;

	bar.loadPoints();

}
void Player::shieldCoolDown(GLFWwindow* window)
{
	if (stats.shieldCoolDownTimer > 0)
	{
		stats.shieldCoolDownTimer--;
		return;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		stats.shieldUpTimer++;
	}
	if (stats.shieldUpTimer > stats.shieldCoolDown)
	{
		stats.shieldCoolDownTimer = stats.shieldCoolDown;
		stats.shieldOn = false;
		stats.shieldUpTimer = 0;
	}
}
void Player::fireBullet(GLFWwindow* window, Camera& camera, Client& client)
{
	if (guns.shootDelay > 0)
	{
		guns.shootDelay--;
		return;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !stats.shieldOn)
	{
		guns.shootDelay = 10;

		//calculate shoot direction
		glm::vec2 mousePos = getMousePos(window);
		glm::vec2 shootDirection = glm::normalize(mousePos - camera.viewSpace(movement.center));

		//create and send bullet
		createBullet(client, shootDirection, 0.7, 0.2, 30, false);
	}
}

void Player::createBullet(Client& client, glm::vec2 shootDirection, float width, float height, float speed, bool bounce)
{
	Complex bulletRotation = Complex::normalize(shootDirection);
	glm::vec2 bulletPos = movement.center;

	//send bullet to server
	struct bulPacket
	{
		glm::vec2 velocity;
		glm::vec2 pos;
		Complex rotation;
		float width;
		float height;
		bool bounce;
	};
	bulPacket nBullet;
	nBullet.velocity = shootDirection * speed;
	nBullet.pos = bulletPos;
	nBullet.rotation = bulletRotation;
	nBullet.width = width;
	nBullet.height = height;
	nBullet.bounce = bounce;
	Message output("bullet");
	output.add("addBullet");
	output.addSimple(nBullet);
	client.addEvent(output, false);
	return;
}

void Player::updateLazer(GLFWwindow* window, Camera& camera, std::vector<Square>& map, std::vector<Player>& players)
{
	if (guns.gun != Gun::lazer || guns.lazerDelay > 0)
	{
		return;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !stats.shieldOn)
	{
		glm::vec2 mousePos = getMousePos(window);
		glm::vec2 lazerRay = glm::normalize(mousePos - camera.viewSpace(movement.center));
		this->guns.lazer.updateLine(movement.center, getLazerRay(map, players, lazerRay));
		this->guns.lazerOn = true;
		guns.lazer.setColor(visual.color);
	}
	if (guns.lazerOn && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		guns.lazerDelay = 2;
		guns.lazer.setColor(glm::vec3(1));
	}
}
void Player::shootLazer(GLFWwindow* window, Camera& camera, Client& client, std::vector<Square>& map, std::vector<Player>& players)
{
	if (guns.lazerDelay > 0)
	{
		guns.lazerDelay--;
		if(guns.lazerDelay == 0)
		{
			guns.lazerOn = false;

			//send lazer shoot to server
			Message output("bullet");
			output.add("lazer");
			output.add("shoot");
			client.addEvent(output, false);
			guns.gun = Gun::normal;
		}
	}

	//send update lazer to server
	this->guns.lazer.updateLine(movement.center, guns.lazer.p2);
	Message output("bullet");
	output.add("lazer");
	output.add("update");
	output.addSimple(guns.lazerOn);
	output.addSimple(guns.lazer.p2);
	output.addSimple(guns.lazer.color);
	client.addUpdate(output);
}

void Player::shotgun(GLFWwindow* window, Camera& camera, Client& client)
{
	if (guns.ammo < 1)
	{
		guns.gun = Gun::normal;
		return;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !stats.shieldOn)
		guns.ableToFire = true;
	if (guns.ableToFire && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		guns.ableToFire = false;
		guns.ammo--;

		//calculate shoot direction
		glm::vec2 mousePos = getMousePos(window);
		glm::vec2 shootDirection = glm::normalize(mousePos - camera.viewSpace(movement.center));
		createBullet(client, shootDirection, 0.7, 1.0, 50, false);
	}
}

void Player::minigun(GLFWwindow* window, Camera& camera, Client& client)
{
	if (guns.ammo == 0)
	{
		guns.gun = Gun::normal;
		return;
	}

	//minigun cooldown
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE || stats.shieldOn)
	{
		guns.minigunCooldown = 40;
		return;
	}
	if (guns.minigunCooldown > 0)
	{
		guns.minigunCooldown--;
		return;
	}

	//shoot cooldown
	if (guns.shootDelay > 0)
	{
		guns.shootDelay--;
		return;
	}
	guns.shootDelay = 2;

	//shoot bullet
	//calculate shoot direction
	glm::vec2 mousePos = getMousePos(window);
	glm::vec2 shootDirection = glm::normalize(mousePos - camera.viewSpace(movement.center));

	//create and send bullet
	createBullet(client, shootDirection, 0.3, 0.15, 30, true);
	guns.ammo--;
}

glm::vec2 Player::getLazerRay(std::vector<Square>& map, std::vector<Player>& players, glm::vec2 ray)
{
	std::vector<glm::vec2> points;
	for (int i=0; i< map.size(); i++)
	{
		std::vector<glm::vec2> pointsToAdd = map[i].intersectLine(movement.center, ray);
		points.insert(points.end(), pointsToAdd.begin(), pointsToAdd.end());
	}
	for (int i = 0; i < players.size(); i++)
	{
		std::vector<glm::vec2> pointsToAdd = players[i].visual.sprite.intersectLine(movement.center, ray);
		points.insert(points.end(), pointsToAdd.begin(), pointsToAdd.end());
	}

	//find closest point
	float minDistance = 999;
	glm::vec2 closestPoint = glm::vec2(0);
	for (glm::vec2& point : points)
	{
		float distanceFromCenter = glm::distance(movement.center, point);
		if (distanceFromCenter < minDistance)
		{
			minDistance = distanceFromCenter;
			closestPoint = point;
		}
	}
	return closestPoint;
}

void Player::sendFallDamage(Client& client)
{
	if (movement.fallHeight < 25)
		return;

	Message output("player");
	output.add("fallDamage");
	client.addEvent(output, false);
}
