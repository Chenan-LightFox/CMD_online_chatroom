#include "../header_file/Chatroom.h"

extern std::mutex cout_mutex;

void ChatRoom::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    for (auto user : users) {
        std::cout << "[" << roomName << "] To " << user->username << ": " << message << std::endl;
    }
}