#include "../header_file/Chatroom.h"
#include "../header_file/ChatServer.h"
#include "../header_file/MessagePacket.h"

extern std::mutex cout_mutex;

void ChatRoom::broadcast(MessagePacket& message) {
    for (auto user : users) {
        if (user->isConnected) {
            ChatServer::sendToClient(user->socket, message);
        }
    }
}

