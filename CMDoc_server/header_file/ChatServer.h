#pragma once

#include <winsock2.h>
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>

class ChatServer {
private:
	SOCKET serverSocket;
	int port;
	std::atomic<bool> running;
	std::vector<std::thread> threads;
public:
	ChatServer(int _port) : port(_port) {};
	ChatServer::~ChatServer() {
		closesocket(serverSocket);
		WSACleanup();
	}

	void start();
	void stop();

	void handleClient(SOCKET clientSocket);
};