#pragma once

#include <winsock2.h>
#include <string>
#include <iostream>
#include <sstream>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include "../header_file/User.h"

class ChatServer {
private:
	SOCKET serverSocket;
	int port;
	std::atomic<bool> running;
	std::vector<std::thread> threads;
	std::map<std::string, User*> registeredUsers;
	std::map<SOCKET, User*> onlineUsers;
public:
	ChatServer(int _port) : port(_port) {};
	ChatServer::~ChatServer() {
		closesocket(serverSocket);
		WSACleanup();
	}

	void start();
	void stop();

	void handleClient(SOCKET clientSocket);
	void handle_client_command(SOCKET clientSocket);
};