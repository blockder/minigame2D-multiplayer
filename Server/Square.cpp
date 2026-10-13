#include"Square.h"
#include<vector>

Square::Square(glm::vec2 mid, float width, float height)
{
	this->mid = mid;
	this->rotation = Complex(1, 0);
	this->width = width;
	this->height = height;
}

bool Square::touch(Square& other)
{
	bool interX = mid.x + width > other.mid.x - other.width && mid.x - width < other.mid.x + other.width;
	bool interY = mid.y + height > other.mid.y - other.height && mid.y - height < other.mid.y + other.height;
	return interX && interY;
}

bool Square::inside(Square& other)
{
	std::vector<glm::vec2> myPoints;
	myPoints.push_back(worldSpace(glm::vec2(-width, height)));
	myPoints.push_back(worldSpace(glm::vec2(width, height)));
	myPoints.push_back(worldSpace(glm::vec2(width, -height)));
	myPoints.push_back(worldSpace(glm::vec2(-width, -height)));

	std::vector<glm::vec2> otherPoints;
	otherPoints.push_back(other.worldSpace(glm::vec2(-other.width, other.height)));
	otherPoints.push_back(other.worldSpace(glm::vec2(other.width, other.height)));
	otherPoints.push_back(other.worldSpace(glm::vec2(other.width, -other.height)));
	otherPoints.push_back(other.worldSpace(glm::vec2(-other.width, -other.height)));

	//for each edge check which points are inside
	for (int i = 0; i < myPoints.size(); i++)
	{
		bool insideEdge = false;
		glm::vec2 normal = getNormal(myPoints[i], myPoints[(i+1)%4]);

		//check all points
		for (int k = 0; k < otherPoints.size(); k++)
		{
			//check if point is inside edge
			if (glm::dot(normal, otherPoints[k] - myPoints[i]) < 0)
			{
				insideEdge = true;
				break;
			}
		}

		//if no points is inside return false
		if (insideEdge == false)
			return false;
	}
	return true;
}
std::vector<glm::vec2> Square::intersectLine(glm::vec2 p, glm::vec2 direction)
{
	std::vector<glm::vec2> points;

	//take care of edge cases
	if (direction.x == 0)
	{
		if (p.x > mid.x - width && p.x < mid.x + width)
		{
			points.push_back(glm::vec2(p.x, mid.y + height));
			points.push_back(glm::vec2(p.x, mid.y - height));
		}
		return points;
	}
	if (direction.y == 0)
	{
		if (p.y > mid.y - height && p.y < mid.y + height)
		{
			points.push_back(glm::vec2(mid.x + width, p.y));
			points.push_back(glm::vec2(mid.x - width, p.y));
		}
		return points;
	}

	//main algorithm
	//find tx 
	float tx1 = (mid.x - width - p.x) / direction.x;
	float tx2 = (mid.x + width - p.x) / direction.x;
	if (tx1 < 0 && tx2 < 0)
		return points;

	//find ty
	float ty1 = (mid.y - height - p.y) / direction.y;
	float ty2 = (mid.y + height - p.y) / direction.y;
	if (ty1 < 0 && ty2 < 0)
		return points;


	//find points
	if (tx1 > ty1 && tx1 < ty2 || tx1 > ty2 && tx1 < ty1)
		points.push_back(p + tx1 * direction);

	if (tx2 > ty1 && tx2 < ty2 || tx2 > ty2 && tx2 < ty1)
		points.push_back(p + tx2 * direction);

	if (ty1 > tx1 && ty1 < tx2 || ty1 > tx2 && ty1 < tx1)
		points.push_back(p + ty1 * direction);

	if (ty2 > tx1 && ty2 < tx2 || ty2 > tx2 && ty2 < tx1)
		points.push_back(p + ty2 * direction);

	return points;
}
glm::vec2 Square::getNormal(glm::vec2 p1, glm::vec2 p2)
{
	glm::vec2 vec = glm::normalize(p2 - p1);
	return glm::vec2(-vec.y, vec.x);
}
glm::vec2 Square::worldSpace(glm::vec2 point)
{
	return (Complex(point) * rotation).getVec() + mid;
}

