#include "../header_file/ChatServer.h"
#include "../header_file/ChatHistoryManager.h"
#include "../header_file/Chatroom.h"
#include "../header_file/MatchEngine.h"
#include "../header_file/PrintLog.h"
#include "../header_file/UserDataManager.h"
#include <algorithm>
#include <functional>
#include <mutex>
#include <sstream>
#include <string.h>
#include <string>
#include <utility>
#include <vector>
#include <windows.h>

extern std::mutex cout_mutex;
std::vector<ChatRoom *> rooms;

void ChatServer::sendToClient(SOCKET clientSocket, MessagePacket &message) {
    send(clientSocket, reinterpret_cast<char *>(&message), sizeof(message), 0);
}
void ChatServer::serverMessage(SOCKET clientSocket,
                               const std::string &message) {
    MessagePacket msg("Server", message);
    sendToClient(clientSocket, msg);
}

void ChatServer::handleClientCommand(std::string &command,
                                     SOCKET clientSocket) {
    User *user = onlineUsers[clientSocket];
    // Handle the command
    if (command == "/help") {
        serverMessage(clientSocket,R"(Available commands:
/best-room - find the best room based on your features
/features - find similar users based on features
/help - show this help
/room - manage chat rooms
/usrname - display your username)");
        printInfo(user->username + " executed command /help");
    } else if (command == "/usrname") {
        serverMessage(clientSocket, "You are: " + user->username + "\n");
        printInfo(user->username + " executed command /usrname");
    } else if (command.find("/room") != std::string::npos) {
        std::istringstream iss(command);
        std::string command, argu;
        iss >> command >> argu;

        if (argu == "create") {
            std::string roomName;
            iss >> roomName;
            if (roomName.empty()) {
                serverMessage(clientSocket, "Room name is required!\n");
                return;
            }
            ChatRoom::createRoom(roomName);
            printInfo(user->username + " executed command /room create " + roomName);
        } else if (argu == "list") {
            std::string roomName;
            iss >> roomName;
            if (roomName.empty()) {
                serverMessage(clientSocket, ChatRoom::listRooms());
                printInfo(user->username + " executed command /room list");
                return;
            }
            ChatRoom::getRoomMembers(roomName);
            printInfo(user->username + " executed command /room list " + roomName);
        } else if (argu == "join") {
            std::string roomName;
            iss >> roomName;
            if (roomName.empty()) {
                serverMessage(clientSocket, "Room name is required!\n");
                return;
            }
            if (!(~ChatRoom::roomExists(roomName))) {
                serverMessage(clientSocket, "Room does not exist!\n");
                return;
            }
            user->joinedRoom = ChatRoom::roomExists(roomName);
            rooms[user->joinedRoom]->users.insert(user);
            printInfo(user->username + " executed command /room join " + roomName);
        } else {
            serverMessage(clientSocket, R"(Not enough parameters!
Available commands:
create - Create a new room
list - List all rooms)");
        }
    } else if (command == "/features") {
        std::vector<std::pair<double, std::string>> dist;
        for (auto it = registeredUsers.begin(); it != registeredUsers.end();
             it++) {
            if (it->first == user->username)
                continue;
            std::lock_guard<std::mutex> lock(it->second->featureMutex);
            double sim = Similarity::cosineSimilarity(user->features,
                                                      it->second->features);
            if (sim > 1 || sim < -1)
                continue;
            dist.push_back({sim, it->first});
        }
        std::sort(dist.begin(), dist.end(),
                  std::greater<std::pair<double, std::string>>());
        std::string result = "Top 5 similarity:\n";
        for (int i = 0; i < 5 && i < dist.size(); i++) {
            result +=
                dist[i].second + ": " + std::to_string(dist[i].first) + "\n";
        }
        serverMessage(clientSocket, result);
        printInfo(user->username + " executed command /features");
    } else if (command == "/best-room") {
        std::vector<std::pair<double, std::string>> dist;
        for (int i = 0; i < rooms.size(); i++) {
            //            std::cout << rooms[i]->features.size() << std::endl;
            double sim = Similarity::cosineSimilarity(user->features,
                                                      rooms[i]->features);
            if (sim > 1 || sim < -1)
                continue;
            dist.push_back({sim, rooms[i]->roomName});
        }
        std::sort(dist.begin(), dist.end(),
                  std::greater<std::pair<double, std::string>>());
        std::string result = "Top 3 best rooms:\n";
        for (int i = 0; i < 3 && i < dist.size(); i++) {
            result +=
                dist[i].second + ": " + std::to_string(dist[i].first) + "\n";
        }
        serverMessage(clientSocket, result);
        printInfo(user->username + " executed command /best-room");
    } else {
        serverMessage(clientSocket, "Unknown command.\n");
        printInfo(user->username + " sent an unknown command: " + command);
    }
}

void ChatServer::start() {
    UserDataManager udm;
    udm.loadUsers("user", registeredUsers); // Load user data from file
    ChatRoom::loadRoomList();               // Load room list from file

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

    // Load chat history and get chat features for each room
    for (auto room : rooms) {
        ChatHistoryManager::loadHistory(room, registeredUsers);
        std::thread saveThread(ChatHistoryManager::saveHistory, room);
        saveThread.detach();
    }
    std::thread featureThread(&ChatServer::getFeatures,
                              this); // Start feature extraction thread
    featureThread.detach();

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
    printInfo("A new client connected successfully.");
    MessagePacket packet;
    int result = recv(clientSocket, reinterpret_cast<char *>(&packet),
                      sizeof(packet), 0);
    if (result <= 0) {
        printError("recv failed: " + std::to_string(WSAGetLastError()));
        closesocket(clientSocket);
        return;
    }
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
        UserDataManager udm;
        udm.saveUsers("user", newUser); // Save user data to file
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
        for (const auto &pair : onlineUsers) { // Check if the user is already logged in
            if (pair.second->username == username) {
                serverMessage(clientSocket,
                              "User already logged in from another client.");
                closesocket(clientSocket);
                return;
            }
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
    user->isConnected = true;    // Set user as connected
    user->socket = clientSocket; // Set user socket
    rooms[user->joinedRoom]->users.insert(user);

    try { // The message loop
        while ((result = recv(clientSocket, reinterpret_cast<char *>(&packet),
                              sizeof(packet), 0)) > 0) {
            std::string msg = packet.content;
            strcpy_s(packet.sender, user->username.c_str());
            if (msg[0] == '/') { // Check if the message is a command
                handleClientCommand(msg, clientSocket);
                continue;
            }
            user->recentMessages.push_back(packet);
            {
                std::lock_guard<std::mutex> lock(
                    rooms[user->joinedRoom]->messageMutex);
                rooms[user->joinedRoom]->messages.push_back(packet);
            }
            printInfo("<" + user->username + "> [" +
                      rooms[user->joinedRoom]->roomName + "] " + msg);
            rooms[user->joinedRoom]->broadcast(
                packet); // Broadcast message to users
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
    rooms[user->joinedRoom]->users.erase(user);
    onlineUsers.erase(clientSocket);
    user->isConnected = false;
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

void ChatServer::getFeatures() {

    MatchEngine match("xiandaihanyuchangyongcibiao.txt");
    while (true) {
        std::vector<User *> users;
        for (auto it = registeredUsers.begin(); it != registeredUsers.end();
             it++) {
            users.push_back(it->second);
        }
        match.getUsersFeature(users);
        std::vector<std::vector<User *>> userGroups(rooms.size());
        for (auto user : users) {
            userGroups[user->joinedRoom].push_back(user);
        }
        if (users.size() > 0) {
            for (int i = 0; i < rooms.size(); i++) {
                rooms[i]->getRoomFeatures(users[0]->features.size(),
                                          userGroups[i]);
            }
        }
        Sleep(10000);
    }
}