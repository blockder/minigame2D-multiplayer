#include"Server.h"
#include<glm/glm.hpp>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <iostream>
#include<thread>
#include <chrono>
#include<cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include<vector>
#include"Player.h"
#include"Square.h"
#include"Bullet.h"
#include"PowerUp.h"
#include"Collision.h"


//SERVER
const int tps = 30;
const bool teamMode = true;
const int seed = 34687;

//world objects
std::vector<Square> map;
std::vector<Player> playerList;
std::vector<Bullet> bullets;



//network stuff
Server server;
static int ACKTimer = 0;
static int bulletId = 0;
void handleRequest(Message& message, int client);
void handleUpdates(Message& message, int client);
void handleEvents(Message& message, int client);


//functions
int getPlayer(int playerId);
int getBullet(int bulletId);
void sendPlayerMovement();
void sendBullets();
void destroyBullet(int bulletI);
void collision_bullet_map();
void collision_bullet_player();
void sendLazer(int playerId);
void shootLazer(int player);




int main()
{
	//initialize glfw
	glfwInit();

	//initiate network
	WSADATA wsaData;
	int wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (wsaerr == 0)
		std::cout << "succesful initialization" << std::endl;
	else
		std::cout << "something went wrong with initialization" << std::endl;

	//create server
	server = Server("127.0.0.1", 5555, 1);

	
	//crate map
	map.push_back(Square(glm::vec2(0, -50.0), 50.0, 1.0));
	map.push_back(Square(glm::vec2(0, 50.0), 50.0, 1.0));
	map.push_back(Square(glm::vec2(50, 0), 1.0, 50.0));
	map.push_back(Square(glm::vec2(-50, 0), 1.0, 50.0));
	map.push_back(Square(glm::vec2(0, -34), 5.0, 10.0));
	map.push_back(Square(glm::vec2(-20, -42), 20.0, 2.0));
	map.push_back(Square(glm::vec2(-32, -31), 18.0, 1.0));
	map.push_back(Square(glm::vec2(11, -21), 2.0, 1.0));
	map.push_back(Square(glm::vec2(28, -12), 2.0, 1.0));
	map.push_back(Square(glm::vec2(23, -43), 10.0, 1.0));
	map.push_back(Square(glm::vec2(22, -31), 17.0, 2.0));
	map.push_back(Square(glm::vec2(36, -44), 3.0, 5.0));
	map.push_back(Square(glm::vec2(36, -1), 3.0, 1.0));
	map.push_back(Square(glm::vec2(11, -1), 5.0, 1.0));
	map.push_back(Square(glm::vec2(25, 7), 20.0, 2.0));
	map.push_back(Square(glm::vec2(-25, 7), 20.0, 2.0));
	map.push_back(Square(glm::vec2(-47, -6), 2.0, 4.0));
	map.push_back(Square(glm::vec2(-32, -11), 2.0, 2.0));
	map.push_back(Square(glm::vec2(-23, -16), 2.0, 2.0));
	map.push_back(Square(glm::vec2(-16, -5), 2.0, 2.0));
	map.push_back(Square(glm::vec2(-25, 12), 2.0, 4.0));
	map.push_back(Square(glm::vec2(-38, 24), 11.0, 2.0));
	map.push_back(Square(glm::vec2(-13, 32), 8.0, 2.0));
	map.push_back(Square(glm::vec2(23, 32), 18.0, 2.0));
	map.push_back(Square(glm::vec2(47, 14), 2.0, 1.0));
	map.push_back(Square(glm::vec2(37, 22), 4.0, 1.0));
	map.push_back(Square(glm::vec2(0, 40), 10.0, 1.0));
	map.push_back(Square(glm::vec2(0, 45), 1.0, 5.0));
	map.push_back(Square(glm::vec2(48, 42), 1.0, 1.0));
	map.push_back(Square(glm::vec2(-42, 42), 8.0, 1.0));

	//reset random
	std::srand(seed);

	//initial powerUp
	PowerUp::initialize();
	

	//game loop
	float tick = 1.0f / tps;
	int fpsCounter = 0;
	double lastFrame = 0;
	double prevTime = glfwGetTime();
	while (true)
	{
		//game tick
		
		//handle requests
		Message input;
		int client;
		while (server.getRequest(input, client))
		{
			handleRequest(input, client);

		}
		//world calculation
		for (Bullet& bullet : bullets)
			bullet.update(tick);
		PowerUp::generate(server);
		collision_bullet_map();
		collision_bullet_player();
		PowerUp::checkPlayerTouch(server, playerList);
		for (Player& player : playerList)
			player.updateVulnerbale(server);
		
		
		//send stuff
		sendPlayerMovement();
		sendBullets();

		//actual send
		server.sendUpdates();
		server.sendEvents(false);
		server.sendPinnedMessages();

		

		//sleep for remaining tick
		double currentTime = glfwGetTime();
		double remainingTime = tick + prevTime - currentTime;
		prevTime += tick;
		std::this_thread::sleep_for(std::chrono::microseconds((int)(remainingTime * 1000000)));
	}


	WSACleanup();
	return 0;
}

void handleRequest(Message& megaMessage, int client)
{
	//read message type
	std::string type = megaMessage.read();

	if (type.compare("event") == 0)
	{
		handleEvents(megaMessage, client);
	}
	if (type.compare("update") == 0)
	{
		handleUpdates(megaMessage, client);
	}

}

void handleUpdates(Message& megaMessage, int client)
{
	while (megaMessage.length > 0)
	{
		//read sub messsage
		Message message = megaMessage.readMessage();

		int playerI = getPlayer(client);
		std::string Header = message.read();
		if (Header.compare("player") == 0)
		{
			if (playerI < 0)
				continue;
			Header = message.read();
			if (Header.compare("movement") == 0)
			{
				glm::vec2 newCenter = message.readSimple<glm::vec2>();
				playerList[playerI].center = newCenter;
				playerList[playerI].sprite.mid = newCenter;
				continue;
			}
			if (Header.compare("shield") == 0)
			{
				//update shield for player
				bool shieldOn = message.readSimple<bool>();
				Square newShield = message.readSimple<Square>();

				playerList[playerI].shield = newShield;
				playerList[playerI].shieldOn = shieldOn;

				//send update to all players
				Message output("player");
				output.addSimple(client);
				output.add("shield");
				output.addSimple(shieldOn);
				output.addSimple(newShield);
				server.addUpdate(output, client, ALL);
				continue;
			}
			if (Header.compare("shieldCooDownTimer") == 0)
			{
				int coolDownTimer = message.readSimple<int>();
				Message output("player");
				output.addSimple(client);
				output.add("shieldCoolDownTimer");
				output.addSimple(coolDownTimer);
				server.addUpdate(output, client, ALL);
				continue;
			}
		}

		if (Header.compare("bullet") == 0)
		{
			Header = message.read();
			if (Header.compare("lazer") == 0)
			{
				Header = message.read();
				if (Header.compare("update") == 0)
				{
					bool lazerON = message.readSimple<bool>();
					glm::vec2 newLazerRay = message.readSimple<glm::vec2>();
					glm::vec3 lazerColor = message.readSimple<glm::vec3>();

					//update stats
					playerList[playerI].lazerOn = lazerON;
					playerList[playerI].lazer = newLazerRay;
					playerList[playerI].lazerColor = lazerColor;

					//send to all the other players
					sendLazer(client);
					continue;
				}
			}
		}
	}
}

void handleEvents(Message& megaMessage, int client)
{
	while (megaMessage.length > 0)
	{
		//read sub messsage
		Message message = megaMessage.readMessage();
		int messageId = message.readSimple<int>();

		int playerI = getPlayer(client);
		std::string Header = message.read();
		if (Header.compare("ACK") == 0)
		{
			int pinId = message.readSimple<int>();
			server.unPinMessage(pinId);
			continue;
		}

		if (Header.compare("player") == 0)
		{
			Header = message.read();
			if (Header.compare("disconnect") == 0)
			{
				std::cout << "disconnect player" << std::endl;
				//delete player and client server side
				if(playerI > -1)
					playerList.erase(playerList.begin() + playerI);
				server.DisconnectClient(client);

				//tell clients to delete player
				Message output("player");
				output.addSimple(client);
				output.add("disconnect");
				server.addEvent(output, -1, ALL, false);
				continue;
			}
			if (Header.compare("newPlayer") == 0)
			{
				glm::vec3 playerColor = message.readSimple<glm::vec3>();
				playerList.push_back(Player(client, playerColor));

				//send newPlayer to all clients
				Message output("player");
				output.addSimple(client);
				output.add("newPlayer");
				output.addSimple(playerColor);
				server.addEvent(output, client, ALL, false);

				//send players to new player
				for (int i = 0; i < playerList.size() - 1; i++)
				{
					Message output("player");
					output.addSimple(playerList[i].client);
					output.add("newPlayer");
					output.addSimple(playerList[i].color);

					server.addEvent(output, client, ONE, false);
				}
				std::cout << "online players: " << playerList.size() << std::endl;
				server.sendACK(client, messageId);
				continue;
			}
			if (Header.compare("fallDamage") == 0)
			{
				playerList[playerI].hitPlayer(server);
			}
		}

		if (Header.compare("bullet") == 0)
		{
			Header = message.read();
			if (Header.compare("addBullet") == 0)
			{
				//recieve new bullet
				struct bulPacket
				{
					glm::vec2 velocity;
					glm::vec2 pos;
					Complex rotation;
					float width;
					float height;
					bool bounce;
				};
				bulPacket packet = message.readSimple<bulPacket>();
				Square hitbox(packet.pos, packet.width, packet.height);
				hitbox.rotation = packet.rotation;
				bulletId++;

				Bullet newBullet = Bullet(hitbox, packet.velocity, bulletId, client, packet.bounce);
				bullets.push_back(newBullet);

				//send new bullet to clients
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
				bul nBullet;
				nBullet.id = newBullet.id;
				nBullet.playerId = newBullet.playerId;
				nBullet.mid = newBullet.hitbox.mid;
				nBullet.rot = newBullet.hitbox.rotation;
				nBullet.width = newBullet.hitbox.width;
				nBullet.height = newBullet.hitbox.height;
				nBullet.velocity = newBullet.velocity;
				nBullet.color = playerList[playerI].color;
				Message output("bullet");
				output.add("addBullet");
				output.addSimple(nBullet);
				server.addEvent(output, -1, ALL, true);
				return;
			}
			if (Header.compare("lazer") == 0)
			{
				Header = message.read();
				if (Header.compare("shoot") == 0)
				{
					shootLazer(playerI);
				}
			}
		}
	}
}

int getPlayer(int playerId)
{
	for (int i = 0; i < playerList.size(); i++)
	{
		if (playerList[i].client == playerId)
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


void sendPlayerMovement()
{
	for(Player& player : playerList)
	{
		Message output("player");
		output.addSimple(player.client);
		output.add("movement");
		output.addSimple(player.center);
		server.addUpdate(output, player.client , ALL);
	}
}
void sendBullets()
{
	if (bullets.size() == 0)
		return;
	//create vector of bullet positions and id
	struct bulPacket
	{
		int id;
		glm::vec2 pos;
		Complex rotation;
	};
	std::vector<bulPacket> bulletsUpdate;
	for (Bullet& bullet : bullets)
	{
		bulPacket b;
		b.id = bullet.id;
		b.pos = bullet.hitbox.mid;
		b.rotation = bullet.hitbox.rotation;
		bulletsUpdate.push_back(b);
	}

	//send positions to clients
	Message output("bullet");
	output.add("movement");
	output.add(bulletsUpdate);
	server.addUpdate(output, -1, ALL);
}
void destroyBullet(int bulletI)
{
	Message output("bullet");
	output.add("destroy");
	output.addSimple(bullets[bulletI].id);
	server.addEvent(output, -1, ALL, true);

	bullets.erase(bullets.begin() + bulletI);
}
void collision_bullet_map()
{
	for (int i=0; i<bullets.size(); i++)
	{
		std::vector<Contact> contacts;
		Bullet& bullet = bullets[i];
		for (Square& block : map)
		{
			if (!bullet.bounce && bullet.hitbox.touch(block))
			{
				destroyBullet(i);
				i--;
				break;
			}

			//bounce bullet
			std::vector<glm::vec2> insidePoints;
			if (bullet.bounce && Collision::insideBlock(block, bullet.hitbox, insidePoints))
			{
				contacts.push_back(Collision::generateContact(block, insidePoints));
				bullet.life--;
				break;
			}
		}
		Collision::resolveContacts(contacts, bullet);
	}
}
void collision_bullet_player()
{
	for (Player& player : playerList)
	{
		for (int i = 0; i < bullets.size(); i++)
		{
			Bullet& bullet = bullets[i];
			if (bullet.playerId == player.client)
				continue;

			//check so bullets won't hit teamates
			bool sameTeam = teamMode && player.color == playerList[getPlayer(bullet.playerId)].color;

			//check if touches shield
			if (player.shieldOn && player.shield.inside(bullet.hitbox))
			{
				destroyBullet(i);
				i--;
				continue;
			}
			if (bullet.hitbox.touch(player.sprite))
			{
				if (!sameTeam)
				{
					player.knockback(server, bullet.velocity);
					player.hitPlayer(server);
				}
				

				destroyBullet(i);
				i--;
				continue;
			}
		}
	}
}



void sendLazer(int playerId)
{
	int playerI = getPlayer(playerId);
	if (playerI < 0)
		return;
	Player& player = playerList[playerI];
	Message output("player");
	output.addSimple(player.client);
	output.add("updateLazer");
	output.addSimple(player.lazerOn);
	output.addSimple(player.lazer);
	output.addSimple(player.lazerColor);
	server.addUpdate(output, player.client, ALL);
	
}
void shootLazer(int playerI)
{
	//see if lazer pointer is close to other player
	Player& player = playerList[playerI];
	for (int i = 0; i < playerList.size(); i++)
	{
		Player& otherPlayer = playerList[i];
		if (i == playerI)
			continue;

		//teams mode
		if (teamMode && player.color == otherPlayer.color)
			continue;
		if (otherPlayer.sprite.intersectLine(player.center, player.lazer -player.center).size() > 0)
		{
			if (!otherPlayer.invulnerable)
			{
				//hit player
				Message output("player");
				output.addSimple(otherPlayer.client);
				output.add("hitFromLazer");
				server.addEvent(output, otherPlayer.client, ONE, true);
				otherPlayer.hitPlayer(server);
				break;
			}
		}
	}
}
