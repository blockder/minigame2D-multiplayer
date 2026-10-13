#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<glm/glm.hpp>

using std::vector;
using std::string;

class Message
{
public:
	std::vector<char> buffer;
	size_t length;

	Message();
	Message(string header);
	Message(const vector<char>& buffer);

	//add
	template<typename T>
	void addSimple(T msg);
	template<typename T>
	void add(std::vector<T>& vector);
	void add(string string);
	void addMessage(Message& other);

	//add front
	template<typename T>
	void addSimpleFront(T msg);
	void addFront(string string);
	

	
	//get
	template<typename T>
	T readSimple();
	std::string read();
	template<typename T>
	std::vector<T> readVector();
	Message readMessage();
private:
};

template<typename T>
inline void Message::addSimple(T msg)
{
	//coppies raw bytes into buffer
	char* bytes = (char*) & msg;
	buffer.insert(buffer.end(), bytes, bytes + sizeof(T));

	//update size
	length += sizeof(T);
}
template<typename T>
inline void Message::addSimpleFront(T msg)
{
	//coppies raw bytes into buffer
	char* bytes = (char*) & msg;
	buffer.insert(buffer.begin(), bytes, bytes + sizeof(T));

	//update size
	length += sizeof(T);
}

template<typename T>
inline void Message::add(std::vector<T>& vector)
{
	size_t vecSize = vector.size() * sizeof(T);
	
	//add message size
	addSimple<size_t>(vecSize);

	//coppies raw bytes into buffer
	char* bytes = (char*)vector.data();
	buffer.insert(buffer.end(), bytes, bytes + vecSize);

	//update size
	length += vecSize;
}


template<typename T>
inline T Message::readSimple()
{
	T body; 
	std::memcpy(&body, buffer.data(), sizeof(T));

	//delete part we coppied
	buffer.erase(buffer.begin(), buffer.begin() + sizeof(T));
	length -= sizeof(T);
	return body;
}

template<typename T>
inline std::vector<T> Message::readVector()
{
	std::vector<T> result;

	//check if messgage is big enough
	if (length < sizeof(T))
		std::cout << "try to read vector from empty message" << std::endl;

	//get vector size
	size_t vecSize = readSimple<size_t>();
	if (length < vecSize)
		std::cout << "try to read vector from empty message" << std::endl;

	//set size for result
	result.resize(vecSize / sizeof(T));

	//coppy bytes into vector
	std::memcpy(result.data(), buffer.data(), vecSize);

	//delete part we coppied
	buffer.erase(buffer.begin(), buffer.begin() + vecSize);

	length -= vecSize;
	return result;

}

struct MessageTo
{
	int id;
	int client;
	int attemptsLeft;
	Message message;

	MessageTo() = default;
	MessageTo(Message& message, int client)
	{
		this->message = message;
		this->client = client;
		id = -1;
		attemptsLeft = 5;
	}
};
