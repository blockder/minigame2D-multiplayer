#include "PowerUp.h"

int PowerUp::nextID;
int PowerUp::cooldown;
std::vector<Power> PowerUp::powerUps;
std::vector<glm::vec2> PowerUp::powerUpsLocations;
std::vector<PowerType> PowerUp::powerTypes;

void PowerUp::initialize()
{
	cooldown = 600;
	nextID = 0;

	//create powerups locations
	powerUpsLocations.push_back(glm::vec2(8, 2));
	powerUpsLocations.push_back(glm::vec2(47, 45));
	powerUpsLocations.push_back(glm::vec2(47, 17));
	powerUpsLocations.push_back(glm::vec2(41, -47));
	powerUpsLocations.push_back(glm::vec2(31, -47));
	powerUpsLocations.push_back(glm::vec2(-16, -1));
	powerUpsLocations.push_back(glm::vec2(0, 22));
	powerUpsLocations.push_back(glm::vec2(-47, 35));
	powerUpsLocations.push_back(glm::vec2(-32, -15));
	powerUpsLocations.push_back(glm::vec2(-43, -25));
	powerUpsLocations.push_back(glm::vec2(-13, -47));
	powerUpsLocations.push_back(glm::vec2(7, -35));
	powerUpsLocations.push_back(glm::vec2(6, 43));
	powerUpsLocations.push_back(glm::vec2(-36, 45));
	powerUpsLocations.push_back(glm::vec2(-19, 36));
	powerUpsLocations.push_back(glm::vec2(43, -15));
	

	//create power types
	powerTypes.push_back(PowerType::shotgun);
	powerTypes.push_back(PowerType::minigun);
	powerTypes.push_back(PowerType::lazer);
	powerTypes.push_back(PowerType::health);
}

void PowerUp::generate(Server& server)
{
	if (cooldown > 0)
	{
		cooldown--;
		return;
	}
	cooldown = 200;
	if (powerUpsLocations.size() == 0)
		return;

	//create powerUp parameters
	int powerUpLocI = std::rand() % powerUpsLocations.size();
	glm::vec2 powerUpPos = powerUpsLocations[powerUpLocI];
	PowerType powerType = powerTypes[std::rand() % powerTypes.size()];

	//erase loc
	powerUpsLocations.erase(powerUpsLocations.begin() + powerUpLocI);

	//create powerUp
	Power powerUp(Square(powerUpPos, 2, 2), powerType);
	powerUps.push_back(powerUp);
	
	//send to clients
	Message output("map");
	output.add("addPowerUp");
	output.addSimple(powerUp.id);
	output.addSimple(powerUp.block.mid);
	output.addSimple(powerUp.power);
	server.addEvent(output, -1, ALL, false);
	
}

void PowerUp::checkPlayerTouch(Server& server, std::vector<Player>& playerList)
{
	for (Player& player : playerList)
	{
		for (int i=0; i< powerUps.size(); i++)
		{
			if (player.sprite.touch(powerUps[i].block))
			{
				//send player powerup
				Message givePower("player");
				givePower.addSimple(player.client);
				givePower.add("powerUp");
				givePower.addSimple(powerUps[i].power);
				server.addEvent(givePower, player.client, ONE, true);

				//send delete powerUp
				Message deletePowerUp("map");
				deletePowerUp.add("destroyPowerUp");
				deletePowerUp.addSimple(powerUps[i].id);
				server.addEvent(deletePowerUp, -1, ALL, true);

				//delete powerUp
				powerUpsLocations.push_back(powerUps[i].block.mid);
				powerUps.erase(powerUps.begin() + i);
				break;
			}
		}
	}
}
