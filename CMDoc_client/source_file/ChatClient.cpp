#include "../header_file/ChatClient.h"
#include "../header_file/PrintLog.h"
#include <iostream>
#include <string>

void ChatClient::receiveLoop(SOCKET clientSocket) {
    MessagePacket packet{};
    while (true) {
        int ret = recv(clientSocket, reinterpret_cast<char *>(&packet),
                       sizeof(packet), 0);
        if (ret <= 0)
            break;
        tm localtime{};
        localtime_s(&localtime, &packet.timestamp);
        std::string timestr=
            std::to_string(localtime.tm_hour) + ":" +
            std::to_string(localtime.tm_min) + ":" +
            std::to_string(localtime.tm_sec);
        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << timestr << ": \n";
            std::cout << "<" << packet.sender << "> " << packet.content << std::endl;
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
        PrintError("Connection failed\n");
        return;
    }
    PrintInfo("Connected to server\n");
    clientThread = std::thread([&]() { this->receiveLoop(clientSocket); });
    clientThread.detach();
}
void ChatClient::stop() {}