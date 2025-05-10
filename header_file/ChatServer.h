#pragma once

#include <winsock2.h>
#include <string>
#include <iostream>
#include <mutex>

class ChatServer {
private:
	SOCKET serverSocket;
	int port;
public:
	ChatServer(int _port) : port(_port) {};
	ChatServer::~ChatServer() {
		closesocket(serverSocket);
		WSACleanup();
	}

	void start();
	// void stop();
	// void sendMessage(const std::string& message);
	// void receiveMessage(std::string& message);

};