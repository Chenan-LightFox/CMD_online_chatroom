#include "../header_file/ChatServer.h"
#include "../header_file/PrintLog.h"

extern std::mutex cout_mutex;

void ChatServer::handle_client_command(SOCKET clientSocket) {
	User* user = onlineUsers[clientSocket];

	// Get the content of the command
	char buffer[1024];
	int result = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (result <= 0) return;

	std::string command(buffer, result);

	// Handle the command
	if (command == "/help") {
		std::string helpText = "Available commands:\n"
			"/help - show this help\n"
			"/usrname - display your username\n";
		send(clientSocket, helpText.c_str(), helpText.size(), 0);
		PrintInfo(user->username + "executed command /help");
	}
	else if (command == "/usrname") {
		std::string reply = "You are: " + user->username + "\n";
		send(clientSocket, reply.c_str(), reply.size(), 0);
		PrintInfo(user->username + " executed command /usrname");
	}
	else {
		send(clientSocket, "Unknown command.\n", 18, 0);
		PrintInfo(user->username + " sent an unknown command: " + command);
	}
}

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

	// Send message when the client connects
	send(clientSocket, "Welcome! Please register or login.\nType '/register' or '/login':\nType '/help' for help.\n", 89, 0);
	result = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (result <= 0) { // Check if the client is unconnected or errors
		closesocket(clientSocket); // Then close the socket
		return;
	}

	// Process the command
	std::string input(buffer, result);
	std::istringstream iss(input);
	std::string command, username, password;
	iss >> command >> username >> password;

	if (command == "/register") {
		if (registeredUsers.count(username)) { // Check if the username already exists
			send(clientSocket, "Username already exists.\n", 26, 0);
			closesocket(clientSocket);
			return;
		}
		User* newUser = new User(username, password);
		registeredUsers[username] = newUser;
		onlineUsers[clientSocket] = newUser;
		send(clientSocket, "Registration successful.\n", 26, 0);
		PrintInfo("New user registered: " + username);
	}
	else if (command == "/login") {
		if (!registeredUsers.count(username) || registeredUsers[username]->password != password) { // Check if the username exists and password is correct
			send(clientSocket, "Invalid username or password.\n", 31, 0);
			closesocket(clientSocket);
			return;
		}
		onlineUsers[clientSocket] = registeredUsers[username];
		send(clientSocket, "Login successful.\n", 19, 0);
		PrintInfo("User logged in: " + username);
	}
	else {
		send(clientSocket, "Unknown command.\n", 18, 0);
		closesocket(clientSocket);
		return;
	}

	try {
		while ((result = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
			std::lock_guard<std::mutex> lock(cout_mutex);
			std::string msg(buffer, result);
			if (msg[0] == '/') { // Check if the message is a command
				handle_client_command(clientSocket);
				continue;
			}
			PrintInfo("[" + onlineUsers[clientSocket]->username + "] " + msg);
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

	if (onlineUsers.count(clientSocket)) {
		PrintInfo("User disconnected: " + onlineUsers[clientSocket]->username);
		onlineUsers.erase(clientSocket);
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