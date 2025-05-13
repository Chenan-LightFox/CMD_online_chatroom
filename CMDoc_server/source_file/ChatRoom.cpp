#include "../header_file/Chatroom.h"
#include "../header_file/ChatServer.h"
#include "../header_file/MessagePacket.h"

extern std::mutex cout_mutex;

void ChatRoom::broadcast(const std::string& sender, const std::string& message) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    for (auto user : users) {
        if (user->socket != -1) {
            ChatServer::send_to_client(user->socket, message);
        }
    }
}