#include "../header_file/ChatServer.h"
#include "../header_file/PrintLog.h"

extern std::mutex cout_mutex;

void ChatServer::start() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		PrintError("WSAStartup failed");
		return;
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		PrintError("Socket creation failed");
		WSACleanup();
		return;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		PrintError("Bind failed");
		stop();
		return;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		PrintError("Listen failed");
		stop();
		return;
	}

	running = true; // Set server state to running
	PrintInfo("Server started on port " + std::to_string(port));

	while (running) {
		if (!running) break; // Check if server is still running before accepting new connections
		SOCKET clientSocket = accept(serverSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			PrintError("Accept failed: " + std::to_string(WSAGetLastError()));
			continue;
		}

		try {
			threads.emplace_back(&ChatServer::handleClient, this, clientSocket);
		} catch (const std::exception& e) {
			PrintError("Failed to create thread: " + std::string(e.what()));
			closesocket(clientSocket);
		} catch (...) {
			PrintError("Unknown error occurred while creating thread.");
			closesocket(clientSocket);
		}
	}

	for (auto& t : threads) {
		if (t.joinable()) t.join();
	}
}

void ChatServer::handleClient(SOCKET clientSocket) {
	char buffer[1024];
	int result;
	try {
		while ((result = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
			std::lock_guard<std::mutex> lock(cout_mutex);
			PrintInfo("Received message: " + std::string(buffer, result));
			send(clientSocket, buffer, result, 0);
		}
		if (result == SOCKET_ERROR) {
			PrintError("recv failed: " + std::to_string(WSAGetLastError()));
		}
	} catch (const std::exception& e) {
		PrintError("Exception in handleClient: " + std::string(e.what()));
	} catch (...) {
		PrintError("Unknown error in handleClient.");
	}
	closesocket(clientSocket);
}

void ChatServer::stop() {
	if (running) {
		running = false;
		shutdown(serverSocket, SD_BOTH);
		closesocket(serverSocket);
		PrintInfo("Server stopped.");
	}
}