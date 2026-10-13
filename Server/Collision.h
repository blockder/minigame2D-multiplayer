#pragma once
#include<glm/glm.hpp>
#include <iostream>
#include<vector>
#include"Square.h"
#include"Bullet.h"


struct Contact
{
	glm::vec2 normal;
	float penetration;

	Contact() = default;
	Contact(glm::vec2 normal, float penetration)
		: normal(normal), penetration(penetration) {
	}
};

class Collision
{
public:
	
	static bool insideBlock(Square& block, Square& hitbox, std::vector<glm::vec2>& pointsInside);
	static Contact generateContact(Square& block, std::vector<glm::vec2> points);
	static void resolveContacts(std::vector<Contact>& contacts, Bullet& bullet);
private:
	static glm::vec2 getNormal(glm::vec2 p1, glm::vec2 p2);
};