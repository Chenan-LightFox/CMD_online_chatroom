#include "../header_file/ChatHistoryManager.h"

void ChatHistoryManager::saveMessage(const std::string &roomName, const MessagePacket &message) {
    std::ofstream msgFile("messages/" + roomName + ".dat", std::ios::app);
    if (!msgFile.is_open())
        return;

    msgFile << "[" << std::ctime(&message.timestamp) << "] <" << message.sender
            << "> " << message.content << "\n";

    msgFile.close();
}