#include "../header_file/MessagePacket.h"
#include <atomic>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>

class ChatClient {
  private:
    SOCKET clientSocket;
    std::string ip;
    short port;
    std::mutex cout_mutex;
    
  public:
    ChatClient(std::string ip, int port) : ip(ip), port(port) {};
    ~ChatClient() {
        closesocket(clientSocket);
        WSACleanup();
    }

    void start();
    void stop();
    void receiveLoop(SOCKET);
};