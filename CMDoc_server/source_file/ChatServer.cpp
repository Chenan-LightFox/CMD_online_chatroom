#include "../header_file/ChatServer.h"

std::mutex cout_mutex;

void ChatServer::start() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed" << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	}

	std::cout << "Server started on port " << port << std::endl;

	while (true) {
		SOCKET clientSocket = accept(serverSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept failed. Error: " << WSAGetLastError() << std::endl;
			continue;
		}
		std::thread(&ChatServer::handleClient, this, clientSocket).detach();
	}
}

void ChatServer::handleClient(SOCKET clientSocket) {
	char buffer[1024];
	int result;
	while ((result = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		{
			std::lock_guard<std::mutex> lock(cout_mutex);
			std::cout << "Received: " << std::string(buffer, result) << std::endl;
		}
		send(clientSocket, buffer, result, 0);
	}
	closesocket(clientSocket);
}