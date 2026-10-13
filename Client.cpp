#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<thread>
#include <chrono>
#include<string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include"Shader.h"
#include"Mesh.h"
#include"Square.h"
#include"player.h"
#include"client.h"
#include"Bullet.h"
#include"Camera.h"
#include"Minimap.h"
#include"Line.h"
#include"PowerUp.h"
#include"Collision.h"

//window settings
GLFWwindow* window;
Camera playerCamera;
Camera worldView;
static int screenWidth = 800;
static int screenHeight = 800;
const bool fullScreen = false;
const int fps = 60;
const int tps = 30;
const float tickDuration = 1.0 / tps;
const float frameDuration = 1.0 / fps;

//network stuff
Client client;

//world objects
std::vector<Player> players;
std::vector<Square> map;
std::vector<Bullet> bullets;
std::vector<Square> hearts;
std::vector<PowerUp> powerUps;


//functions
int getPlayer(int playerId);
int getBullet(int bulletId);
void handleRequest(Message& message, Player& player);
void handleEventRequest(Message& message, Player& player);
void handleUpdateRequest(Message& message, Player& player);
void gameTick(float tickDuration, Player& player);
void frameUpdate(float frameDuration, Player& player);
void draw(Shader& shaderProgram, Minimap& minimap, Player& player);
void drawScene(Shader& shaderProgram, Camera& camera, Player& player);


int main()
{
	//initialize glfw
	glfwInit();
	//specify the version of opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//tel opengl we are using only core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//create a glfwWindow with size and name
	if(fullScreen)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		screenWidth = mode->width;
		screenHeight = mode->height;
	}
	window = glfwCreateWindow(screenWidth, screenHeight, "openGl", NULL, NULL); 
	if (window == NULL)
	{
		std::cout << "falied to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();

	//openGL settings
	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//initiate network
	WSADATA wsaData;
	int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaerr == 0)
		std::cout << "succesful initialization" << std::endl;
	else
		std::cout << "something went wrong with initialization" << std::endl;

	//connect to server
	client.Connect("127.0.0.1:5555", 1);


	playerCamera = Camera(glm::vec3(0), 0.04f, screenWidth, screenHeight);
	worldView = Camera(glm::vec3(0), 0.02, screenWidth, screenHeight);

	//create minimap
	Minimap minimap = Minimap(glm::vec2(0.75, -0.75), 0.25, 0.25, screenWidth, screenHeight);
	
	//creat default shader
	Shader shaderProgram("shaders/default.vert", "shaders/default.frag");

	

	
	//create player
	glm::vec3 playerColor = glm::vec3(0.5, 0.00, 0.27);
	Square square(glm::vec2(0), 1.0, 1.0, "textures/blank.png", playerColor);
	Square shield(glm::vec2(0), 0.5, 1.0, "textures/blank.png", playerColor);
	Square coolDownBar(glm::vec2(0), 0.9, 0.1, "textures/blank.png", glm::vec3(1));
	Player player = Player(square, shield, coolDownBar, playerColor);
	Message newPlayer("player");
	newPlayer.add("newPlayer");
	newPlayer.addSimple(playerColor);
	client.addEvent(newPlayer, true);


	//crate map
	map.push_back(Square(glm::vec2(0,-50.0), 50.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(0,50.0), 50.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(50, 0), 1.0, 50.0, "textures/blank.png", glm::vec3(0.1, 0.1, 0.1)));
	map.push_back(Square(glm::vec2(-50,0), 1.0, 50.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(0,-34), 5.0, 10.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-20,-42), 20.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-32,-31), 18.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(11,-21), 2.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(28,-12), 2.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(23,-43), 10.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(22,-31), 17.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(36,-44), 3.0, 5.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(36,-1), 3.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(11,-1), 5.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(25,7), 20.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-25,7), 20.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-47,-6), 2.0, 4.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-32,-11), 2.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-23,-16), 2.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-16,-5), 2.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-25,12), 2.0, 4.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-38,24), 11.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-13,32), 8.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(23,32), 18.0, 2.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(47,14), 2.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(37,22), 4.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(0,40), 10.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(0,45), 1.0, 5.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(48,42), 1.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	map.push_back(Square(glm::vec2(-42,42), 8.0, 1.0, "textures/blank.png", glm::vec3(0.1,0.1,0.1)));
	

	//create heart bar
	for (int i = 0; i < 10; i++)
	{
		hearts.push_back(Square(glm::vec2(-1 + 0.05 + i*0.1, -1 + 0.05), 0.05, 0.05, "textures/heart_icon.png", glm::vec3(1)));
	}


	//game loop
	int fpsCounter = 0;
	double lastTick = 0;
	double lastFrame = 0;
	double lastSecond = 0;
	while (!glfwWindowShouldClose(window) && player.stats.health > 0)
	{
		double currentTime = glfwGetTime();

		//update title
		if (currentTime - lastSecond > 1.0)
		{
			lastSecond = currentTime;
			std::string title = "OPENGL FPS:" + std::to_string(fpsCounter);
			glfwSetWindowTitle(window, title.c_str());
			fpsCounter = 0;
		}

		//game tick
		if (currentTime - lastTick > tickDuration)
		{
			lastTick = currentTime;
			gameTick(tickDuration, player);
		}

		//frame
		if(currentTime - lastFrame > frameDuration)
		{
			lastFrame = currentTime;
			fpsCounter++;
			frameUpdate(frameDuration, player);
			draw(shaderProgram, minimap, player);
		}

		
		// Take care of all GLFW events
		glfwPollEvents();
	}

	//disconnect client
	client.Disconnect();

	//close window, glfw, network
	glfwDestroyWindow(window);
	glfwTerminate();
	WSACleanup();
	return 0;
}

void gameTick(float tickDuration, Player& player)
{
	//get update from server
	std::vector<Message> messages = client.getRequests();
	for(Message& input : messages)
	{
		handleRequest(input, player);
	}
	
	//nextTarget for players
	for (Player& player : players)
	{
		player.nextTarget(tickDuration);
	}

	//send pos to server
	player.sendMovement(client);
	player.sendFallDamage(client);
	
	//fire bullet
	player.shoot(window, playerCamera, client, map, players);

	//send shield
	player.shieldCoolDown(window);
	player.sendShield(client);

	//actuall send
	client.sendEvents();
	client.sendUpdates();
	client.sendPinnedMessages();
}

void frameUpdate(float frameDuration, Player& player)
{
	//update our player
	player.update(window, playerCamera, frameDuration, map);

	//update other players
	for (Player& player : players)
	{
		player.interpolate(frameDuration);
		player.updateCoolDownBar();
	}

	//update camera pos
	playerCamera.moveTo(player.movement.center);

	//update bullets
	for (Bullet& bullet : bullets)
	{
		bullet.update(frameDuration);
	}
	player.updateLazer(window, playerCamera, map, players);

}

void draw(Shader& shaderProgram, Minimap& minimap, Player& player)
{
	//bind minimap framebuffer
	minimap.bind();

	//render minimap
	drawScene(shaderProgram, worldView, player);

	//bind back to screen desplay
	minimap.unBind();

	
	drawScene(shaderProgram, playerCamera, player);

	//draw minimap
	minimap.square.draw(shaderProgram);

	//draw heart bar
	for (int i = 0; i < player.stats.health; i++)
		hearts[i].draw(shaderProgram);

	

	// display what was drawn
	glfwSwapBuffers(window);
}

void drawScene(Shader& shaderProgram, Camera& camera, Player& player)
{
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw map
	for (Square& block : map)
		block.draw(shaderProgram, camera);

	//draw powerUps
	for (PowerUp& powerUp : powerUps)
		powerUp.block.draw(shaderProgram, camera);


	//draw bullets
	for (Bullet& bullet : bullets)
		bullet.hitbox.draw(shaderProgram, camera);


	//draw players
	player.visual.sprite.draw(shaderProgram, camera);
	if(player.visual.showBar)player.visual.coolDownBar.draw(shaderProgram, camera);
	if (player.stats.shieldOn) player.visual.shield.draw(shaderProgram, camera);
	if (player.guns.lazerOn)  player.guns.lazer.draw(shaderProgram, camera); 

	for (Player& player : players)
	{
		player.visual.sprite.draw(shaderProgram, camera);
		if (player.visual.showBar)player.visual.coolDownBar.draw(shaderProgram, camera);
		if (player.stats.shieldOn) player.visual.shield.draw(shaderProgram, camera);
		if (player.guns.lazerOn) player.guns.lazer.draw(shaderProgram, camera);
	}

}

void handleRequest(Message& message, Player& player)
{
	//read message type
	std::string type = message.read();

	if (type.compare("event") == 0)
	{
		handleEventRequest(message, player);
	}

	if (type.compare("update") == 0)
	{
		handleUpdateRequest(message, player);
	}
	
}

void handleEventRequest(Message& MegaMessage, Player& player)
{
	while(MegaMessage.length > 0)
	{
		//read sub message
		Message message = MegaMessage.readMessage();

		//read message id
		int messageId = message.readSimple<int>();

		std::string Header = message.read();
		if (Header.compare("ACK") == 0)
		{
			int pinId = message.readSimple<int>();
			client.unpin(pinId);
			continue;
		}
		if (Header.compare("player") == 0)
		{
			//get player id
			int playerId = message.readSimple<int>();
			int playerI = getPlayer(playerId);

			Header = message.read();
			if (Header.compare("newPlayer") == 0)
			{
				glm::vec3 playerColor = message.readSimple<glm::vec3>();
				Square square(glm::vec2(0), 1.0, 1.0, "textures/blank.png", playerColor);
				Square shield(glm::vec2(0), 0.5, 1.0, "textures/blank.png", playerColor);
				Square coolDownBar(glm::vec2(0), 0.9, 0.1, "textures/blank.png", glm::vec3(1));
				players.push_back(Player(square, shield, coolDownBar, playerColor));
				players[players.size() - 1].id = playerId;
				continue;
			}

			if (Header.compare("disconnect") == 0)
			{
				if (playerI > -1) 
					players.erase(players.begin() + playerI);
				continue;
			}

			if (Header.compare("invulnerable") == 0)
			{
				//std::cout << "hit" << std::endl;
				bool invulnerable = message.readSimple<bool>();

				//if someone else hit
				if (playerI > -1)
				{
					if (invulnerable)
					{
						players[playerI].visual.sprite.setColor(glm::vec3(1, 0, 0));
						players[playerI].stats.health--;
					}
					else players[playerI].visual.sprite.setColor(players[playerI].visual.color);

					players[playerI].stats.invulnerable = invulnerable;
					players[playerI].stats.health--;
					continue;
				}

				//if this player hit
				if (invulnerable)
				{
					player.visual.sprite.setColor(glm::vec3(1, 0, 0));
					player.stats.health--;
				}
				else player.visual.sprite.setColor(player.visual.color);
				player.stats.invulnerable = invulnerable;
				continue;
			}

			if (Header.compare("hitFromLazer") == 0)
			{
				std::cout << "hit from lazer" << std::endl;
				if (playerI < 0)
				{
					player.stats.health = 0;
					hearts.clear();
				}
				continue;
			}

			if (Header.compare("powerUp") == 0)
			{
				PowerType power = message.readSimple<PowerType>();
				switch (power)
				{
				case PowerType::lazer:
					player.guns.gun = Gun::lazer;
					break;
				case PowerType::shotgun:
					player.guns.ammo = 20;
					player.guns.gun = Gun::shotgun;
					break;
				case PowerType::minigun:
					player.guns.ammo = 20;
					player.guns.gun = Gun::minigun;
					break;
				case PowerType::health:
					if (player.stats.health < 10)
						player.stats.health++;
					break;
				default:
					break;
				}
				client.sendACK(messageId);
			}
			continue;
		}

		if (Header.compare("bullet") == 0)
		{
			Header = message.read();
			if (Header.compare("addBullet") == 0)
			{
				struct bul
				{
					int id;
					int playerId;
					glm::vec2 mid;
					Complex rot;
					float width;
					float height;
					glm::vec2 velocity;
					glm::vec3 color;
				};
				bul newBullet = message.readSimple<bul>();

				//return if bullet already exists
				if (getBullet(newBullet.id) != -1)
					continue;
				Square hitbox(newBullet.mid, newBullet.width, newBullet.height, "textures/blank.png", newBullet.color);
				hitbox.rotation = newBullet.rot;
				bullets.push_back(Bullet(hitbox, newBullet.velocity, newBullet.id));
				client.sendACK(messageId);
				continue;
			}
			
			if (Header.compare("destroy") == 0)
			{
				int bulletId = message.readSimple<int>();
				int i = getBullet(bulletId);
				if (i >= 0)
				{
					bullets.erase(bullets.begin() + i);
					client.sendACK(messageId);
				}
				continue;
			}
		}

		if (Header.compare("map") == 0)
		{
			std::string header2 = message.read();
			if (header2.compare("addPowerUp") == 0)
			{
				int powerId = message.readSimple<int>();
				glm::vec2 pos = message.readSimple<glm::vec2>();
				PowerType type = message.readSimple<PowerType>();
				powerUps.push_back(PowerUp(powerId, type, pos));
				client.sendACK(messageId);
				continue;
			}
			if (header2.compare("destroyPowerUp") == 0)
			{
				int powerId = message.readSimple<int>();
				for (int i = 0; i < powerUps.size(); i++)
				{
					if (powerUps[i].id == powerId)
					{
						powerUps.erase(powerUps.begin() + i);
						client.sendACK(messageId);
						break;
					}
				}
				continue;
			}
		}

	}
}
void handleUpdateRequest(Message& MegaMessage, Player& player)
{
	while (MegaMessage.length > 0)
	{
		//read sub message
		Message message = MegaMessage.readMessage();

		std::string Header = message.read();
		if (Header.compare("player") == 0)
		{
			//get playerid
			int playerId = message.readSimple<int>();
			int playerI = getPlayer(playerId);

			Header = message.read();
			if (Header.compare("movement") == 0)
			{
				glm::vec2 newCenter = message.readSimple<glm::vec2>();
				if (playerI > -1)
				{
					players[playerI].addTarget(newCenter);
				}
				continue;
			}
				

			if (Header.compare("knockback") == 0)
			{
				glm::vec2 knockback = message.readSimple<glm::vec2>();
				if (playerI < 0)
				{
					player.movement.velocity += knockback * 20.0f;
				}
				continue;
			}

			if (Header.compare("shield") == 0)
			{
				if (playerI < 0)
					continue;
				bool shieldOn = message.readSimple<bool>();
				struct ShPacket
				{
					glm::vec2 mid;
					Complex rotation;
					float width;
					float height;
				};
				ShPacket packet = message.readSimple<ShPacket>();
				Square newShield(packet.mid, packet.width, packet.height, "textures/blank.png", players[playerI].visual.color);
				newShield.rotation = packet.rotation;
				players[playerI].stats.shieldOn = shieldOn;
				players[playerI].visual.shield = newShield;
				continue;
			}

			if (Header.compare("shieldCoolDownTimer") == 0)
			{
				if (playerI < 0)
					continue;
				int cooldownTimer = message.readSimple<int>();
				players[playerI].stats.shieldCoolDownTimer = cooldownTimer;
				continue;
			}

			if (Header.compare("updateLazer") == 0)
			{
				if (playerI < 0)
					continue;

				//update someone else's lazer
				Player& otherPlayer = players[playerI];
				otherPlayer.guns.lazerOn = message.readSimple<bool>();
				otherPlayer.guns.lazer.updateLine(otherPlayer.movement.center, message.readSimple<glm::vec2>());
				otherPlayer.guns.lazer.setColor(message.readSimple<glm::vec3>());
				continue;
			}

		}

		if (Header.compare("bullet") == 0)
		{
			Header = message.read();
			if (Header.compare("movement") == 0)
			{
				struct bulPacket
				{
					int id;
					glm::vec2 pos;
					Complex rotation;
				};
				std::vector<bulPacket> bulletsUpdate = message.readVector<bulPacket>();

				//update position for each bullet
				for (bulPacket& packet : bulletsUpdate)
				{
					int i = getBullet(packet.id);
					if (i > -1)
					{
						Bullet& bullet = bullets[i];
						bullet.velocity = (packet.pos - bullet.hitbox.pos) / tickDuration;
						bullet.hitbox.rotation = packet.rotation;
					}
				}
				continue;
			}
		}

	}
}

int getPlayer(int playerId)
{
	for (int i = 0; i < players.size(); i++)
	{
		if (players[i].id == playerId)
		{
			return i;
		}
	}
	return -1;
}
int getBullet(int bulletId)
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i].id == bulletId)
		{
			return i;
		}
	}
	return -1;
}

