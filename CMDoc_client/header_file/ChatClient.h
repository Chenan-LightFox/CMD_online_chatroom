#pragma once
#include "../header_file/MessagePacket.h"
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

extern std::mutex coutMutex;
class ChatClient {
  private:
    SOCKET clientSocket;
    std::string ip;
    short port;
    std::thread clientThread;
    std::string userName;

  public:
    ChatClient(std::string ip, int port, const std::string &userName)
        : ip(ip), port(port), userName(userName) {};
    ~ChatClient() {
        closesocket(clientSocket);
        WSACleanup();
    }

    void start();
    void stop();
    void sendPackage(MessagePacket packet) {
        if (packet.content[0] == '\0' || packet.content[0] == '\n')
            return;
        int result = send(clientSocket, reinterpret_cast<char *>(&packet),
                          sizeof(packet), 0);
        if (result == SOCKET_ERROR)
            throw std::runtime_error("Send failed");
    }
    void receiveLoop(SOCKET);
};