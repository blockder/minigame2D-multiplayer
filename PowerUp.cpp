#include "PowerUp.h"

PowerUp::PowerUp(int id, PowerType power, glm::vec2 position)
{
	this->id = id;
	this->type = power;

	switch (power)
	{
	case PowerType::lazer:
	{
		block = Square(position, 2, 2, "textures/lazer_icon.png", glm::vec3(1));
		break;
	}
	case PowerType::shotgun:
	{
		block = Square(position, 2, 2, "textures/shotgun_icon.png", glm::vec3(1));
		break;
	}
	case PowerType::minigun:
	{
		block = Square(position, 2, 2, "textures/minigun_icon.png", glm::vec3(1));
		break;
	}
	case PowerType::health:
	{
		block = Square(position, 2, 2, "textures/heart_icon.png", glm::vec3(1));
		break;
	}
	default:
		break;
	}

}
