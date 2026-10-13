#include"Client.h"




void Client::Connect(std::string ip_port, int async)
{
    nextID = 0;

    //create addres for server
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int pos = ip_port.find(":");
    int iAddr = getaddrinfo(ip_port.substr(0, pos).c_str(), ip_port.substr(pos + 1, 4).c_str(), &hints, &serverAddress);
    if (iAddr != 0) {
        printf("getaddrinfo failed: %d\n", iAddr);
        return;
    }

    //create UDP socket
    connection = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (connection == INVALID_SOCKET)
    {
        std::cout << "failed to create connection" << std::endl;
        return;
    }
    std::cout << "connected successfully" << std::endl;

    //set sync mode
    u_long Async = async;
    ioctlsocket(connection, FIONBIO, &Async);
}

void Client::Disconnect()
{
    //tell server client disconnected
    Message Disconnect;
    Disconnect.addSimple(-1);
    Disconnect.add("player");
    Disconnect.add("disconnect");
    Disconnect.addSimpleFront(Disconnect.length);
    Disconnect.addFront("event");
    send(Disconnect);

    //close socket
    if (connection != INVALID_SOCKET)
        closesocket(connection);
    connection = INVALID_SOCKET;

    //erase server adress
    freeaddrinfo(serverAddress);
}
bool Client::isConnected()
{
    return connection != INVALID_SOCKET;
}




void Client::send(Message& message)
{
    //send to server
    int sendE = sendto(connection, message.buffer.data(), message.length, 0, serverAddress->ai_addr, serverAddress->ai_addrlen);
    if (sendE == SOCKET_ERROR)
    {
        std::cout << "failed to send mesage to server" << std::endl;
    }
}

std::vector<Message> Client::getRequests()
{
    std::vector<Message> messages;
    std::vector<int> messageOrder;
    Message input;
    while (getRequest(input))
    {
        int priority = input.readSimple<int>();
        bool inserted = false;
        for (int i = 0; i < messageOrder.size(); i++)
        {
            if (priority <= messageOrder[i])
            {
                messages.insert(messages.begin() + i, input);
                messageOrder.insert(messageOrder.begin() + i, priority);
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
            messages.push_back(input);
            messageOrder.push_back(priority);
        }
    }
    return messages;
}

void Client::addUpdate(Message& message)
{
    updates.push_back(MessageTo(message, -1));
}

void Client::addEvent(Message& message, bool pin)
{
    MessageTo messageTo(message, -1);
    giveId(messageTo, pin);
    events.push_back(messageTo);
}

void Client::sendUpdates()
{
    std::vector<Message> output = combine(updates);
    for (Message& message : output)
    {
        message.addFront("update");
        send(message);
    }

    //clear updates
    updates.clear();
}

void Client::sendEvents()
{
    std::vector<Message> output = combine(events);
    for (Message& message : output)
    {
        message.addFront("event");
        send(message);
    }

    //clear events
    events.clear();
}

void Client::giveId(MessageTo& message, bool pin)
{
    if (message.id > -1)
        return;

    //add id
    message.id = nextID;
    nextID++;
    message.message.addSimpleFront(message.id);

    if (pin)
        ack.push_back(message);
}
void Client::sendACK(int messageId)
{
    Message output("ACK");
    output.addSimple(messageId);
    addEvent(output, false);
}
void Client::sendPinnedMessages()
{
    if (ack.size() == 0)
        return;

    //timer to resend
    if (resendTimer > 0)
    {
        resendTimer--;
        return;
    }
    resendTimer = 40;

    //update number of attemps
    for (int i = 0; i < ack.size(); i++)
    {
        ack[i].attemptsLeft--;
        if (ack[i].attemptsLeft == 0)
        {
            ack.erase(ack.begin() + i);
            i--;
        }
    }

    //combine into big messages
    std::vector<Message> output = combine(ack);

    //send all messages
    for (int i = 0; i < output.size(); i++)
    {
        //add title
        Message& message = output[i];
        message.addFront("event");
        send(message);
    }
}
void Client::unpin(int messageId)
{
    for (int i = 0; i < ack.size(); i++)
    {
        if (ack[i].id == messageId)
        {
            ack.erase(ack.begin() + i);
            return;
        }
    }
}

std::vector<Message> Client::combine(std::vector<MessageTo>& messages)
{
    std::vector<Message> output;
    output.push_back(Message());
    int slot = 0;
    for (MessageTo& message : messages)
    {
        if (output[slot].length > maxMessageSize)
        {
            slot++;
            output[slot] = Message();
        }
        output[slot].addMessage(message.message);
    }
    return output;
}

bool Client::getRequest(Message& message)
{
    //create buffer to store message
    char buffer[1500];

    int bytesReceived = recvfrom(connection, buffer, 1500, 0, NULL, NULL);

    //return if no data avalible/ not enough
    if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
        return false;

    //return message
    message = Message(std::vector<char>(buffer, buffer + bytesReceived));
    return true;

}


