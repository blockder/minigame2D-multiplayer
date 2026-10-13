#pragma once
#include"player.h"

struct Contact
{
	glm::vec2 normal;
	float penetration;

	Contact() = default;
	Contact(glm::vec2 normal, float penetration) 
		: normal(normal), penetration(penetration) {}
};

class Collision
{
public:
	static std::vector<Contact> CollisionDetection(Square& body, std::vector<Square>& map);
	static void resolveContacts(std::vector<Contact>& contacts, Player& player);
private:
	static bool generateContact(Square& body, Square& other, Contact& contact);
};