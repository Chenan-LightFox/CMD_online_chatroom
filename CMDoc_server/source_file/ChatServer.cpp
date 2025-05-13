#include "../header_file/ChatServer.h"
#include "../header_file/printLog.h"
#include "../header_file/ChatRoom.h"
#include <string.h>
#include <vector>
extern std::mutex cout_mutex;
std::vector<ChatRoom> rooms;

void ChatServer::sendToClient(SOCKET clientSocket, const MessagePacket &message) {
    MessagePacket msg=message;
    send(clientSocket, reinterpret_cast<char*>(&msg), sizeof(msg), 0);
}
void ChatServer::serverMessage(SOCKET clientSocket, const std::string &message){
    MessagePacket msg("Server",message);
    sendToClient(clientSocket, msg);
}

void ChatServer::handleClientCommand(SOCKET clientSocket) {
    User *user = onlineUsers[clientSocket];

    // Get the content of the command
    MessagePacket packet;
    int result = recv(clientSocket, reinterpret_cast<char *>(&packet),
                      sizeof(packet), 0);
    if (result <= 0) {
        closesocket(clientSocket);
        return;
    }
    std::string input(packet.content);
    std::istringstream iss(input);
    std::string command;
    iss >> command;

    // Handle the command
    if (command == "/help") {
        serverMessage(clientSocket,
                       "Available commands:\n/help - show this help\n/usrname "
                       "- display your username\n");
        printInfo(user->username + "executed command /help");
    } else if (command == "/usrname") {
        serverMessage(clientSocket, "You are: " + user->username + "\n");
        printInfo(user->username + " executed command /usrname");
    } else {
        serverMessage(clientSocket, "Unknown command.\n");
        printInfo(user->username + " sent an unknown command: " + command);
    }
}

void ChatServer::start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printError("WSAStartup failed");
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printError("Socket creation failed");
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) ==
        SOCKET_ERROR) {
        printError("Bind failed");
        stop();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printError("Listen failed");
        stop();
        return;
    }

    running = true; // Set server state to running
    printInfo("Server started on port " + std::to_string(port));
    
    rooms.push_back(ChatRoom("Lobby"));
    while (running) {
        if (!running)
            break; // Check if server is still running before accepting new
                   // connections
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printError("Accept failed: " + std::to_string(WSAGetLastError()));
            continue;
        }

        try {
            threads.emplace_back(&ChatServer::handleClient, this, clientSocket);
        } catch (const std::exception &e) {
            printError("Failed to create thread: " + std::string(e.what()));
            closesocket(clientSocket);
        } catch (...) {
            printError("Unknown error occurred while creating thread.");
            closesocket(clientSocket);
        }
    }

    for (auto &t : threads) {
        if (t.joinable())
            t.join();
    }
}

void ChatServer::handleClient(SOCKET clientSocket) {
    // Send message when the client connects
    serverMessage(clientSocket,
                   "Welcome! Please register or login.\nType '/register' or "
                   "'/login':\nType '/help' for help.\n");
    printInfo("Say hello to the new client.");
    MessagePacket packet;
    int result = recv(clientSocket, reinterpret_cast<char *>(&packet),
                      sizeof(packet), 0);
    // Process the command
    std::string input(packet.content);
    std::istringstream iss(input);
    std::string command, username, password;
    iss >> command >> username >> password;
    if (command == "/register") {
        if (registeredUsers.count(
                username)) { // Check if the username already exists
            serverMessage(clientSocket, "Username already exists.\n");
            closesocket(clientSocket);
            return;
        }
        User *newUser = new User(username, password);
        registeredUsers[username] = newUser;
        onlineUsers[clientSocket] = newUser;
        serverMessage(clientSocket, "Registration successful.\n");
        printInfo("New user registered: " + username);
    } else if (command == "/login") {
        if (!registeredUsers.count(username) ||
            registeredUsers[username]->password !=
                password) { // Check if the username exists and password is
                            // correct
            serverMessage(clientSocket, "Invalid username or password.\n");
            closesocket(clientSocket);
            return;
        }
        onlineUsers[clientSocket] = registeredUsers[username];
        serverMessage(clientSocket, "Login successful.\n");
        printInfo("User logged in: " + username);
    } else {
        serverMessage(clientSocket, "Unknown command.\n");
        closesocket(clientSocket);
        return;
    }

    User *user = onlineUsers[clientSocket];

    try { // The message loop
        while ((result = recv(clientSocket, reinterpret_cast<char *>(&packet),
                              sizeof(packet), 0)) > 0) {
            std::string msg = packet.content;
            if (msg[0] == '/') { // Check if the message is a command
                handleClientCommand(clientSocket);
                continue;
            }
            printInfo("<" + onlineUsers[clientSocket]->username + "> " + msg);
            rooms[user->joinedRoom].broadcast(packet); // Broadcast message to users
        }
        if (result == SOCKET_ERROR) {
            printError("recv failed: " + std::to_string(WSAGetLastError()));
        }
    } catch (const std::exception &e) {
        printError("Exception in handleClient: " + std::string(e.what()));
    } catch (...) {
        printError("Unknown error in handleClient.");
    }

    if (onlineUsers.count(clientSocket)) {
        printInfo("User disconnected: " + onlineUsers[clientSocket]->username);
        onlineUsers.erase(clientSocket);
    }
    rooms[user->joinedRoom].users.erase(user);
    onlineUsers.erase(clientSocket);
    closesocket(clientSocket);
}

void ChatServer::stop() {
    if (running) {
        running = false;
        shutdown(serverSocket, SD_BOTH);
        closesocket(serverSocket);
        printInfo("Server stopped.");
    }
}