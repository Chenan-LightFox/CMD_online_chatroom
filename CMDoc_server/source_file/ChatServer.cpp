#include "../header_file/ChatServer.h"
#include "../header_file/PrintLog.h"
#include <string.h>

extern std::mutex cout_mutex;

void send_to_client(SOCKET clientSocket, const std::string &message) {
    MessagePacket reply;
    reply.set("SERVER", "Registration successful.\n");
    std::string msg = reply.serialize();
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

void ChatServer::handle_client_command(SOCKET clientSocket) {
    User *user = onlineUsers[clientSocket];

    // Get the content of the command
    char buffer[sizeof(MessagePacket)];
    int result = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (result <= 0) {
        closesocket(clientSocket);
        return;
    }
    MessagePacket packet = MessagePacket::deserialize(buffer, result);
    std::string input(packet.content);

    std::string command(buffer, result);

    // Handle the command
    if (command == "/help") {
        send_to_client(clientSocket,
                       "Available commands:\n/help - show this help\n/usrname "
                       "- display your username\n");
        PrintInfo(user->username + "executed command /help");
    } else if (command == "/usrname") {
        send_to_client(clientSocket, "You are: " + user->username + "\n");
        PrintInfo(user->username + " executed command /usrname");
    } else {
        send_to_client(clientSocket, "Unknown command.\n");
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

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {
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
        if (!running)
            break; // Check if server is still running before accepting new
                   // connections
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            PrintError("Accept failed: " + std::to_string(WSAGetLastError()));
            continue;
        }

        try {
            threads.emplace_back(&ChatServer::handleClient, this, clientSocket);
        } catch (const std::exception &e) {
            PrintError("Failed to create thread: " + std::string(e.what()));
            closesocket(clientSocket);
        } catch (...) {
            PrintError("Unknown error occurred while creating thread.");
            closesocket(clientSocket);
        }
    }

    for (auto &t : threads) {
        if (t.joinable())
            t.join();
    }
}

void ChatServer::handleClient(SOCKET clientSocket) {
    MessagePacket packet;
    int result = recv(clientSocket, reinterpret_cast<char *>(&packet), sizeof(packet), 0);

    // Send message when the client connects
    send_to_client(clientSocket,
                   "Welcome! Please register or login.\nType '/register' or "
                   "'/login':\nType '/help' for help.\n");

    // Process the command
    std::string input(packet.content);
    std::istringstream iss(input);
    std::string command, username, password;
    iss >> command >> username >> password;

    if (command == "/register") {
        if (registeredUsers.count(
                username)) { // Check if the username already exists
            send_to_client(clientSocket, "Username already exists.\n");
            closesocket(clientSocket);
            return;
        }
        User *newUser = new User(username, password);
        registeredUsers[username] = newUser;
        onlineUsers[clientSocket] = newUser;
        send_to_client(clientSocket, "Registration successful.\n");
        PrintInfo("New user registered: " + username);
    } else if (command == "/login") {
        if (!registeredUsers.count(username) ||
            registeredUsers[username]->password !=
                password) { // Check if the username exists and password is
                            // correct
            send_to_client(clientSocket, "Invalid username or password.\n");
            closesocket(clientSocket);
            return;
        }
        onlineUsers[clientSocket] = registeredUsers[username];
        send_to_client(clientSocket, "Login successful.\n");
        PrintInfo("User logged in: " + username);
    } else {
        send_to_client(clientSocket, "Unknown command.\n");
        closesocket(clientSocket);
        return;
    }

    try {
        while ((result = recv(clientSocket, reinterpret_cast<char *>(&packet),
                              sizeof(packet), 0)) > 0) {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::string msg(packet.content, result);
            if (msg[0] == '/') { // Check if the message is a command
                handle_client_command(clientSocket);
                continue;
            }
            PrintInfo("[" + onlineUsers[clientSocket]->username + "] " + msg);
            send_to_client(clientSocket, reinterpret_cast<char *>(&packet));
        }
        if (result == SOCKET_ERROR) {
            PrintError("recv failed: " + std::to_string(WSAGetLastError()));
        }
    } catch (const std::exception &e) {
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