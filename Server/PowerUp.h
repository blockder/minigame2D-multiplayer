#pragma once
#include"Server.h"
#include<glm/glm.hpp>
#include <iostream>
#include<cstring>
#include<vector>
#include"Square.h"
#include"Player.h"

struct Power;
enum class PowerType
{
	lazer,
	shotgun,
	minigun,
	health,
};

class PowerUp
{
public:
	static int nextID;
	static int cooldown;
	static std::vector<Power> powerUps;
	static std::vector<glm::vec2> powerUpsLocations;
	static std::vector<PowerType> powerTypes;

	static void initialize();
	static void generate(Server& server);
	static void checkPlayerTouch(Server& server, std::vector<Player>& playerList);
};

struct Power
{
	int id;
	Square block;
	PowerType power;

	Power(Square block, PowerType power)
	{
		id = PowerUp::nextID;
		PowerUp::nextID++;
		this->block = block;
		this->power = power;
	}
};