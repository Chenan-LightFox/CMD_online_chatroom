#include "../header_file/ChatHistoryManager.h"

#include "../header_file/PrintLog.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <windows.h>

namespace fsys = std::filesystem; // Only available in C++17 and later

void ChatHistoryManager::saveHistory(ChatRoom *room) {
    while (true) {
        Sleep(3000);

        if (!fsys::exists("room"))
            fsys::create_directory("room");
        std::ofstream fs("room/" + room->roomName + ".dat",
                         std::ios::binary | std::ios::trunc);
        if (!fs) {
            printError("Failed to open file: " + room->roomName + ".dat");
            throw std::runtime_error("Failed to open file");
            return;
        }
        std::lock_guard<std::mutex> lock(room->messageMutex);
        for (auto msg : room->messages) {
            fs.write(reinterpret_cast<char *>(&msg), sizeof(msg));
            fs.flush();
        }
        fs.close();
    }
}

void ChatHistoryManager::loadHistory(
    ChatRoom *room, std::map<std::string, User *> &registeredUsers) {
    if (!fsys::exists("room"))
        fsys::create_directory("room");
    std::ifstream fs("room/" + room->roomName + ".dat", std::ios::binary);
    if (!fs) {
        printWarning("Failed to open file: " + room->roomName + ".dat");
        return;
    }
    MessagePacket msg;
    while (fs.read(reinterpret_cast<char *>(&msg), sizeof(msg))) {
        std::lock_guard<std::mutex> lock(room->messageMutex);
        room->messages.push_back(msg);
        registeredUsers[msg.sender]->recentMessages.push_back(msg);
    }
    fs.close();
}