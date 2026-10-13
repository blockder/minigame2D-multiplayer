#include"Server.h"
#include<memory>


Server::Server(const char* ip, int port, int async)
{
    nextID = 1;
    msgPriority = 0;
    resendTimer = 40;

    //initialize peers
    peers.reserve(20);

    //create adrees of server
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    InetPtonA(AF_INET, ip, &address.sin_addr);



    //create UDP socket
    listener = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    u_long Async = async;
    ioctlsocket(listener, FIONBIO, &Async);
    if (listener == INVALID_SOCKET)
        std::cout << "failed to create server" << std::endl;
    else
        std::cout << "server is up" << std::endl;

    //bind lisener
    if (bind(listener, (sockaddr*)&address, sizeof(sockaddr_in)) == SOCKET_ERROR)
        std::cout << "failed to bind listener" << std::endl;


}
int Server::getClient(sockaddr_in& clientAddress)
{
    //search to match a clientAddress already registered
    for (int i = 0; i < peers.size(); i++)
    {
        if (clientAddress.sin_family == peers[i].address.sin_family
            && clientAddress.sin_port == peers[i].address.sin_port
            && clientAddress.sin_addr.s_addr == peers[i].address.sin_addr.s_addr)
        {
            peers[i].online = true;
            return i;
        }
    }

    //if not address matched add a new client
    peers.push_back(clientAddress);
    return peers.size() - 1;
}
void Server::DisconnectClient(int client)
{
    if (client < 0 || client >= peers.size())
        return;

    //reset to default value
    peers[client].online = false;
    std::cout << "deleted client " << client << std::endl;
}



void Server::givePriority(Message& message, bool Ordered)
{
    //give priority
    if (Ordered)
    {
        message.addSimpleFront(msgPriority);
        msgPriority++;
    }
    else
        message.addSimpleFront(0);

}
void Server::sendEvents(bool Ordered)
{
    //combine into big messages
    std::vector<MessageTo> output = combine(events);

    //send all messages
    for (MessageTo& toSend : output)
    {
        //add title
        Message& message = toSend.message;
        message.addFront("event");
        send(message, toSend.client, Ordered);
    }

    //delete queue
    events.clear();
}

void Server::sendUpdates()
{
    //combine into big messages
    std::vector<MessageTo> output = combine(updates);

    //send all messages
    for (MessageTo& toSend : output)
    {
        //add tile
        Message& message = toSend.message;
        message.addFront("update");
        send(message, toSend.client, false);
    }

    //delete queue
    updates.clear();
}


void Server::send(Message& message, int client, bool Ordered)
{
    //check to see if client exist
    if (client < 0 || client >= peers.size() || !peers[client].online)
        return;

    //give priority to message
    givePriority(message, Ordered);

    //send to server
    int sendE = sendto(listener, message.buffer.data(), message.length, 0, (sockaddr*)&peers[client].address, sizeof(sockaddr_in));
    if (sendE == SOCKET_ERROR)
    {
        std::cout << "failed to send mesage to server" << std::endl;
        DisconnectClient(client);
    }
}




bool Server::getRequest(Message& message, int& client)
{
    int addressLength = sizeof(sockaddr_in);
    sockaddr_in clientAddress;

    //create buffer to store message
    char buffer[1500];

    int bytesReceived = recvfrom(listener, buffer, 1500, 0, (sockaddr*)&clientAddress, &addressLength);

    //return if no data avalible/ not enough
    if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
        return false;

    //return client
    client = getClient(clientAddress);

    //return message
    message = Message(std::vector<char>(buffer, buffer + bytesReceived));
    return true;
}

void Server::giveId(MessageTo& messageTo, bool pin)
{
    if (messageTo.id > -1)
        return;

    ///give id
    messageTo.id = nextID;
    nextID++;

    //insert id
    messageTo.message.addSimpleFront(messageTo.id);

    //pin
    if (pin)
        ACK.push_back(messageTo);
}


void Server::unPinMessage(int messageId)
{
    for (int i = 0; i < ACK.size(); i++)
    {
        if (ACK[i].id == messageId)
        {
            ACK.erase(ACK.begin() + i);
            return;
        }
    }
}
void Server::sendACK(int client, int messageId)
{
    Message output("ACK");
    output.addSimple(messageId);
    addEvent(output, client, ONE, false);
}
void Server::sendPinnedMessages()
{
    if (ACK.size() == 0)
        return;

    //timer to resend
    if (resendTimer > 0)
    {
        resendTimer--;
        return;
    }
    resendTimer = 40;

    //update number of attemps
    for (int i = 0; i < ACK.size(); i++)
    {
        ACK[i].attemptsLeft--;
        if (ACK[i].attemptsLeft == 0)
        {
            ACK.erase(ACK.begin() + i);
            std::cout << "delete ack" << std::endl;
            i--;
        }
    }
    //combine into big messages

    std::vector<MessageTo> output = combine(ACK);

    //send all messages
    for (int i = 0; i < output.size(); i++)
    {
        //add title
        Message& message = output[i].message;
        message.addFront("event");
        send(message, output[i].client, false);

        std::cout << "ACK " << i << " resent" << std::endl;
    }
}


void Server::addEvent(Message& message, int client, Target target, bool pin)
{
    //add message for client
    if (target == ONE)
    {
        MessageTo messageTo(message, client);
        giveId(messageTo, pin);
        events.push_back(messageTo);
        return;
    }

    if (target == ALL)
    {
        ///add message for every client except one
        for (int i = 0; i < peers.size(); i++)
        {
            if (i == client || !peers[i].online)
                continue;
            MessageTo messageTo(message, i);
            giveId(messageTo, pin);
            events.push_back(messageTo);
        }
    }
}

void Server::addUpdate(Message& message, int client, Target target)
{
    //add message for client
    if (target == ONE)
    {
        updates.push_back(MessageTo(message, client));
        return;
    }

    if (target == ALL)
    {
        ///add message for every client except one
        for (int i = 0; i < peers.size(); i++)
        {
            if (i == client || !peers[i].online)
                continue;
            updates.push_back(MessageTo(message, i));
        }
    }
}

std::vector<MessageTo> Server::combine(std::vector<MessageTo>& messages)
{
    std::vector<MessageTo> output;
    for (MessageTo& message : messages)
    {
        bool found = false;

        //find if mega message already exists for client
        for (int i = 0; i < output.size(); i++)
        {
            if (output[i].client == message.client && output[i].message.length < maxMessageSize)
            {
                //if exist add to it
                output[i].message.addMessage(message.message);
                found = true;
                break;
            }
        }
        if (found) continue;

        //if didn't find add new out message
        MessageTo newSlot = message;
        newSlot.message.addSimpleFront(newSlot.message.length);
        output.push_back(newSlot);

    }
    return output;
}
