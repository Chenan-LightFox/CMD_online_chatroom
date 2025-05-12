#include "../header_file/ChatClient.h"
#include<iostream>


void ChatClient::receiveLoop(SOCKET clientSocket) {
    MessagePacket packet;
    while (true) {
        int ret = recv(clientSocket, reinterpret_cast<char *>(&packet),
                       sizeof(packet), 0);
        if (ret <= 0)
            break;
        std::cout << "[" << packet.sender << "] " << packet.content << "\n";
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

    connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));//TODO:异常处理

    std::thread([&]() { this->receiveLoop(clientSocket); }, clientSocket).detach();
}