#include "../header_file/ChatHistoryManager.h"
#include "../header_file/PrintLog.h"
#include <fstream>
#include <windows.h>
#include <filesystem>

namespace fsys = std::filesystem; // Only available in C++17 and later

std::queue<MessagePacket> messageQueue;
std::mutex messageMutex;

void ChatHistoryManager::saveHistory(const std::string &roomName) {
	while (true) {
		Sleep(3000);
        if (!fsys::exists("room"))
			fsys::create_directory("room");
        std::ofstream fs("room/" + roomName + ".dat",
                         std::ios::binary | std::ios::trunc);
		if (!fs) {
            printError("Failed to open file: " + roomName + ".dat");
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

void ChatHistoryManager::loadHistory(const std::string &roomName) {
    if (!fsys::exists("room"))
        fsys::create_directory("room");
    std::ifstream fs("room/" + roomName + ".dat",
					 std::ios::binary);
	if (!fs) {
        printError("Failed to open file: " + roomName + ".dat");
        throw std::runtime_error("Failed to open file");
		return;
	}
	MessagePacket msg;
	while (fs.read(reinterpret_cast<char *>(&msg), sizeof(msg))) {
		std::lock_guard<std::mutex> lock(messageMutex);
		messageQueue.push(msg);
	}
	fs.close();
}