#include "../header_file/ChatClient.h"
#include "../header_file/PrintLog.h"
#include <mutex>
#include <queue>
#include <stack>
#include <string>

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

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
        printError("Connection failed\n");
        return;
    }
    printInfo("Connected to server\n");
    clientThread = std::thread([&]() { this->receiveLoop(clientSocket); });
    clientThread.detach();
}
void ChatClient::stop() {}