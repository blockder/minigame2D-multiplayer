#pragma once
#include"Client.h"
#include<glm/glm.hpp>
#include <iostream>
#include<cstring>
#include<vector>
#include"Square.h"
#include"Player.h"




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
	int id;
	Square block;
	PowerType type;

	PowerUp(int id, PowerType power, glm::vec2 position);
};