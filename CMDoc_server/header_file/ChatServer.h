#pragma once

#include "../header_file/MessagePacket.h"
#include "../header_file/User.h"
#include <atomic>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>

class ChatServer {
  private:
    SOCKET serverSocket;
    int port;
    std::atomic<bool> running;
    std::vector<std::thread> threads;
    std::map<std::string, User *> registeredUsers;
    std::map<SOCKET, User *> onlineUsers;

  public:
    ChatServer(int _port) : port(_port){};

    void start();
    void stop();

    void handleClient(SOCKET clientSocket);
    void handleClientCommand(std::string &command, SOCKET clientSocket);
    static void sendToClient(SOCKET clientSocket, MessagePacket &message);
    static void serverMessage(SOCKET clientSocket, const std::string &message);
    void getFeatures();
};