#include"Collision.h"

void Collision::resolveContacts(std::vector<Contact>& contacts, Bullet& bullet)
{
	for (Contact& contact : contacts)
	{
		//resolve velocity
		bullet.velocity += contact.normal * glm::dot(-contact.normal, 2.0f * bullet.velocity);

		//resolve penetration
		bullet.hitbox.mid += contact.normal * (contact.penetration + 0.1f);

		//change rotation
		bullet.hitbox.rotation = Complex::normalize(bullet.velocity);
	}
}



Contact Collision::generateContact(Square& block, std::vector<glm::vec2> points)
{
	glm::vec2 normal(0);
	float penetration = 999;

	std::vector<glm::vec2> edges;
	edges.push_back(glm::vec2(0, block.height));
	edges.push_back(glm::vec2(0, -block.height));
	edges.push_back(glm::vec2(block.width, 0));
	edges.push_back(glm::vec2(-block.width, 0));
	
	//for each edge
	for (glm::vec2& edge : edges)
	{
		glm::vec2 localNormal = glm::normalize(edge);
		float miniPen = 0;
		for (glm::vec2 point : points)
		{
			point -= block.mid;

			//check if point is inside
			float distanceEdge = glm::dot(localNormal, edge - point);
			if (distanceEdge < 0)
				continue;
			miniPen = std::max(miniPen, distanceEdge);
			if (miniPen < penetration)
			{
				normal = localNormal;
				penetration = miniPen;
			}
		}
	}

	return Contact(normal, penetration);
}
bool Collision::insideBlock(Square& block, Square& hitbox, std::vector<glm::vec2>& pointsInside)
{
	std::vector<glm::vec2> hitboxPoints;
	hitboxPoints.push_back(hitbox.worldSpace(glm::vec2(-hitbox.width, hitbox.height)));
	hitboxPoints.push_back(hitbox.worldSpace(glm::vec2(hitbox.width, hitbox.height)));
	hitboxPoints.push_back(hitbox.worldSpace(glm::vec2(hitbox.width, -hitbox.height)));
	hitboxPoints.push_back(hitbox.worldSpace(glm::vec2(-hitbox.width, -hitbox.height)));

	std::vector<glm::vec2> blockPoints;
	blockPoints.push_back(block.worldSpace(glm::vec2(0, block.height)));
	blockPoints.push_back(block.worldSpace(glm::vec2(0, -block.height)));
	blockPoints.push_back(block.worldSpace(glm::vec2(block.width, 0)));
	blockPoints.push_back(block.worldSpace(glm::vec2(-block.width, 0)));

	//for each edge check which points are inside
	for (int i = 0; i < blockPoints.size(); i++)
	{
		glm::vec2 normal = glm::normalize(blockPoints[i] - block.mid);
		
		//check all points
		for (int k = 0; k < hitboxPoints.size(); k++)
		{
			//check if point is outside edge
			if (glm::dot(normal, hitboxPoints[k] - blockPoints[i]) > 0)
			{
				hitboxPoints.erase(hitboxPoints.begin() + k);
				k--;
			}
		}

		if (hitboxPoints.size() == 0)
			return false;
	}
	
	pointsInside = hitboxPoints;
	return true;
}

glm::vec2 Collision::getNormal(glm::vec2 p1, glm::vec2 p2)
{
	glm::vec2 vec = glm::normalize(p2 - p1);
	return glm::vec2(-vec.y, vec.x);
}