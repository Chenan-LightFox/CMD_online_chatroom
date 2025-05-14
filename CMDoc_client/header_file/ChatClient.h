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

  public:
    ChatClient(std::string ip, int port) : ip(ip), port(port) {};
    ~ChatClient() {
        closesocket(clientSocket);
        WSACleanup();
    }

    void start();
    void stop();
    void test(MessagePacket packet) {
        send(clientSocket, reinterpret_cast<char *>(&packet), sizeof(packet),
             0);
    }
    void receiveLoop(SOCKET);
};