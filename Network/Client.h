#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<vector>
#include<string>
#include"Message.h"


class Client
{
public:

	void Connect(std::string ip_port, int async);
	void Disconnect();
	bool isConnected();

	//read
	std::vector<Message> getRequests();

	//add
	void addUpdate(Message& message);
	void addEvent(Message& message, bool pin);

	//send
	void sendUpdates();
	void sendEvents();

	//ACK
	void sendACK(int messageId);
	void sendPinnedMessages();
	void unpin(int messageId);

private:
	//connection
	SOCKET connection;
	addrinfo* serverAddress;

	//messages
	std::vector<MessageTo> updates;
	std::vector<MessageTo> events;
	size_t maxMessageSize = 1000;
	std::vector<Message> combine(std::vector<MessageTo>& messages);

	//ACK
	int nextID;
	int resendTimer = 40;
	std::vector<MessageTo> ack;
	void giveId(MessageTo& message, bool pin);

	void send(Message& message);
	bool getRequest(Message& message);
};


