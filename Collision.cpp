#include"Collision.h"

std::vector<Contact> Collision::CollisionDetection(Square& body, std::vector<Square>& map)
{
	 std::vector<Contact> contacts;
	 for (Square& block : map)
	 {
		 Contact potentialContact;
		 if (generateContact(body, block, potentialContact))
			 contacts.push_back(potentialContact);
	 }
	 return contacts;

}

void Collision::resolveContacts(std::vector<Contact>& contacts, Player& player)
{
	for (Contact& contact : contacts)
	{
		//resolve velocity
		player.movement.velocity += contact.normal * glm::dot(-contact.normal, player.movement.velocity);
		player.movement.velocity.y = std::max(player.movement.velocity.y, -3.0f);
		player.movement.fallHeight = player.movement.lastBlockStanding - player.movement.center.y;
		player.movement.lastBlockStanding = player.movement.center.y;
		if (contact.normal == glm::vec2(0, 1))
		{
			player.movement.velocity.x = 0;
			player.movement.jumpsLeft = 2;
		}

		//resolve penetration
		player.movement.center += contact.normal * contact.penetration;

		//update sprite pos
		player.visual.sprite.pos = player.movement.center;
	}
}

bool Collision::generateContact(Square& body, Square& other, Contact& contact)
{
	float smallestPen = 999;
	glm::vec2 currentNormal;

	//check down
	float downDis = (other.pos.y + other.height) - (body.pos.y - body.height);
	if (downDis < 0)	return false;
	if (downDis < smallestPen)
	{
		smallestPen = downDis;
		currentNormal = glm::vec2(0, 1);
	}
	
	//check up
	float upDis = (body.pos.y + body.height) - (other.pos.y - other.height);
	if (upDis < 0)		return false;
	if (upDis < smallestPen)
	{
		smallestPen = upDis;
		currentNormal = glm::vec2(0, -1);
	}

	//check right side
	float rightDis = (other.pos.x + other.width) - (body.pos.x - body.width);
	if (rightDis < 0)		return false;
	if (rightDis < smallestPen)
	{
		smallestPen = rightDis;
		currentNormal = glm::vec2(1, 0);
	}

	//check left side
	float leftDis = (body.pos.x + body.width) - (other.pos.x - other.width);
	if (leftDis < 0)		return false;
	if (leftDis < smallestPen)
	{
		smallestPen = leftDis;
		currentNormal = glm::vec2(-1, 0);
	}

	contact = Contact(currentNormal, smallestPen);
	return true;
}
