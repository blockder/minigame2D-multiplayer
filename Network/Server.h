#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<vector>
#include <string>
#include"Message.h"


struct clientSlot;

enum Target
{
	ALL,
	ONE,
};
class Server
{
public:

	Server() = default;
	Server(const char* ip, int port, int async);

	void DisconnectClient(int client);

	//send and recieve
	void sendEvents(bool Ordered);
	void sendUpdates();
	void sendPinnedMessages();
	bool getRequest(Message& message, int& client);
	void sendACK(int client, int messageId);

	void unPinMessage(int messageId);

	//add message
	void addEvent(Message& message, int client, Target target, bool pin);
	void addUpdate(Message& message, int client, Target target);

private:
	//network
	SOCKET listener;
	sockaddr_in address;
	std::vector<clientSlot> peers;
	int getClient(sockaddr_in& clientAddress);

	//ACK
	std::vector<MessageTo> ACK;
	int nextID;
	int msgPriority;
	int resendTimer;
	void giveId(MessageTo& message, bool pin);

	//message queues
	size_t maxMessageSize = 1000;
	std::vector<MessageTo> events;
	std::vector<MessageTo> updates;

	//send
	void send(Message& message, int client, bool Ordered);
	void givePriority(Message& message, bool Ordered);
	std::vector<MessageTo> combine(std::vector<MessageTo>& messages);
};

struct clientSlot
{
	sockaddr_in address;
	bool online;

	clientSlot(sockaddr_in address)
	{
		this->address = address;
		online = true;
	}
};


