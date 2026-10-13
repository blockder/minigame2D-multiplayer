#pragma once
#include"playerStructure.h"


class Client;
struct Contact;

class Player
{
public:
	int id;

	Stats stats;
	Visual visual;
	Guns guns;
	Movement movement;

	Player() = default;
	Player(Square& sprite, Square& shield, Square& coolDownBar, glm::vec3 color);

	void update(GLFWwindow* window, Camera& camera, float tickDuration, std::vector<Square>& map);
	void interpolate(float tickDuration);
	void addTarget(glm::vec2 newTarget);
	void nextTarget(float time);

	void shoot(GLFWwindow* window, Camera& camera, Client& client, std::vector<Square>& map, std::vector<Player>& players);
	void shieldCoolDown(GLFWwindow* window);
	void sendShield(Client& client);
	void sendMovement(Client& client);
	void updateLazer(GLFWwindow* window, Camera& camera, std::vector<Square>& map, std::vector<Player>& players);
	void updateCoolDownBar();
	void sendFallDamage(Client& client);
	
private:
	glm::vec2 getMousePos(GLFWwindow* window);
	void jumpMechanics(GLFWwindow* window, std::vector<Contact>& contacts);
	void updateShield(GLFWwindow* window, Camera& camera);
	void fireBullet(GLFWwindow* window, Camera& camera, Client& client);
	void createBullet(Client& client, glm::vec2 shootDirection, float width, float height, float speed, bool bounce);
	void shootLazer(GLFWwindow* window, Camera& camera, Client& client, std::vector<Square>& map, std::vector<Player>& players);
	void shotgun(GLFWwindow* window, Camera& camera, Client& client);
	void minigun(GLFWwindow* window, Camera& camera, Client& client);
	glm::vec2 getLazerRay(std::vector<Square>& map, std::vector<Player>& players, glm::vec2 ray);
};