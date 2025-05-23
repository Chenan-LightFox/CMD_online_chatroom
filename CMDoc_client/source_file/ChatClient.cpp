#include "../header_file/ChatClient.h"
#include "../header_file/PrintLog.h"
#include <mutex>
#include <stack>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

std::stack<MessagePacket> messageStack;
std::stack<MessagePacket> messageStackBuf;

void ChatClient::receiveLoop(SOCKET clientSocket) {
    MessagePacket packet{};
    while (true) {
        int ret = recv(clientSocket, reinterpret_cast<char *>(&packet),
                       sizeof(packet), 0);
        if (ret <= 0)
            break;
        {
            std::lock_guard<std::mutex> lock(messageMutex);
            messageStack.push(packet);
        }
    }
}
void ChatClient::start() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    PADDRINFOA addrInfo;
    int ret = GetAddrInfoA(ip.c_str(), std::to_string(port).c_str(), nullptr,
                           &addrInfo);
    if (ret != 0 || addrInfo == nullptr) {
        printError("GetAddrInfoA failed\n");
        return;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    addrInfo->ai_addr->sa_family = AF_INET;

    if (connect(clientSocket, addrInfo->ai_addr,
                sizeof(*(addrInfo->ai_addr)))) {
        printError("Connection failed\n");
        return;
    }
    printInfo("Connected to server\n");
    clientThread = std::thread([&]() { this->receiveLoop(clientSocket); });
    clientThread.detach();
}
void ChatClient::stop() {}