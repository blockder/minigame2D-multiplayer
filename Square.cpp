#include "Square.h"


Square::Square(glm::vec2 pos, float width, float height, const char* texture, glm::vec3 color) : Mesh()
{
	this->pos = pos;
	this->width = width;
	this->height = height;

	
	createTexture(texture);

	//create vertices 
	vertices.push_back(Vertex(glm::vec3(-width, height, 0), color, glm::vec2(0,1)));
	vertices.push_back(Vertex(glm::vec3(width, height, 0), color, glm::vec2(1,1)));
	vertices.push_back(Vertex(glm::vec3(width, -height, 0), color, glm::vec2(1,0)));
	vertices.push_back(Vertex(glm::vec3(-width, -height, 0), color, glm::vec2(0,0)));

	//create indices
	indices = { 0, 1, 2,
				0, 2, 3 };

	loadVBO();
}

void Square::loadPoints()
{
	vertices[0].position = glm::vec3(-width, height, 0);
	vertices[1].position = glm::vec3(width, height, 0);
	vertices[2].position = glm::vec3(width, -height, 0);
	vertices[3].position = glm::vec3(-width, -height, 0);
	loadVBO();
}
bool Square::touch(Square& other)
{
	bool interX = pos.x + width > other.pos.x - other.width && pos.x - width < other.pos.x + other.width;
	bool interY = pos.y + height > other.pos.y - other.height && pos.y - height < other.pos.y + other.height;
	return interX && interY;
}

std::vector<glm::vec2> Square::intersectLine(glm::vec2 p, glm::vec2 direction)
{
	std::vector<glm::vec2> points;
	
	//take care of edge cases
	if (direction.x == 0)
	{
		if (p.x > pos.x - width && p.x < pos.x + width)
		{
			points.push_back(glm::vec2(p.x, pos.y + height));
			points.push_back(glm::vec2(p.x, pos.y - height));
		}
		return points;
	}
	if (direction.y == 0)
	{
		if (p.y > pos.y - height && p.y < pos.y + height)
		{
			points.push_back(glm::vec2(pos.x + width, p.y));
			points.push_back(glm::vec2(pos.x - width, p.y));
		}
		return points;
	}

	//main algorithm
	//find tx 
	float tx1 = (pos.x - width - p.x) / direction.x;
	float tx2 = (pos.x + width - p.x) / direction.x;
	if (tx1 < 0 && tx2 < 0)
		return points;

	//find ty
	float ty1 = (pos.y - height - p.y) / direction.y;
	float ty2 = (pos.y + height - p.y) / direction.y;
	if (ty1 < 0 && ty2 < 0)
		return points;


	//find points
	if (tx1 > ty1 && tx1 < ty2 || tx1 > ty2 && tx1 < ty1)
		points.push_back(p + tx1*direction);
	
	if (tx2 > ty1 && tx2 < ty2 || tx2 > ty2 && tx2 < ty1)
		points.push_back(p + tx2*direction);
	
	if (ty1 > tx1 && ty1 < tx2 || ty1 > tx2 && ty1 < tx1)
		points.push_back(p + ty1*direction);
	
	if (ty2 > tx1 && ty2 < tx2 || ty2 > tx2 && ty2 < tx1)
		points.push_back(p + ty2*direction);

	return points;
}

void Square::setColor(glm::vec3 color)
{
	for (int i = 0; i < vertices.size(); i++)
		vertices[i].color = color;
	loadVBO();
}

glm::vec3 Square::getColor()
{
	return vertices[0].color;
}
