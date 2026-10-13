#include "Message.h"



Message::Message()
{
	length = 0;
}
Message::Message(string header) : Message()
{
	add(header);
}

Message::Message(const vector<char>& buffer) : Message()
{
	this->buffer = buffer;
	this->length = buffer.size();
}




void Message::add(string string)
{
	//add message size
	addSimple<size_t>(string.size());

	//inserts string into buffer as raw bytes
	buffer.insert(buffer.end(), string.begin(), string.end());

	//update size
	length += string.length();
}
void Message::addFront(string string)
{
	//inserts string into buffer as raw bytes
	buffer.insert(buffer.begin(), string.begin(), string.end());

	//add message size
	addSimpleFront<size_t>(string.size());

	//update size
	length += string.length();
}

void Message::addMessage(Message& other)
{
	//add other message size
	addSimple<size_t>(other.length);

	//add buffer
	buffer.insert(buffer.end(), other.buffer.begin(), other.buffer.end());
	
	//add length
	length += other.length;
}




string Message::read()
{
	//check if messgage is big enough
	if (length < sizeof(size_t))
		std::cout << "try to read string from empty message" << std::endl;

	//get string size
	size_t msgSize = readSimple<size_t>();
	if (length < msgSize)
		std::cout << "try to read string from empty message" << std::endl;

	std::string Header(buffer.begin(), buffer.begin() + msgSize);

	buffer.erase(buffer.begin(), buffer.begin() + msgSize);
	length -= msgSize;
	return Header;
}

Message Message::readMessage()
{
	std::vector<char> bytes = readVector<char>();

	return Message(bytes);
}


