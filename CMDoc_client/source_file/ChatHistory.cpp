#include "../header_file/ChatHistory.h"
#include <fstream>
#include <windows.h>
void ChatHistory::saveHistory(const std::string &fileName) {
    while (true) {
        Sleep(3000);
        std::ofstream fs(fileName, std::ios::binary | std::ios::trunc);
        if (!fs) {
            throw std::runtime_error("Failed to open file");
            return;
        }
        std::lock_guard<std::mutex> lock(messageMutex);
        int queueSize = messageQueue.size();
        for (int i = 0; i < queueSize; i++) {
            MessagePacket msg = messageQueue.front();
            messageQueue.pop();
            fs.write(reinterpret_cast<char *>(&msg), sizeof(msg));
            fs.flush();
            messageQueue.push(msg);
        }
        fs.close();
    }
}
void ChatHistory::loadHistory(const std::string &fileName) {
    std::ifstream fs(fileName, std::ios::binary);
    if (!fs) {
        return;
    }
    MessagePacket msg;
    while (fs.read(reinterpret_cast<char *>(&msg), sizeof(msg))) {
        std::lock_guard<std::mutex> lock(messageMutex);
        messageQueue.push(msg);
    }
    fs.close();
}