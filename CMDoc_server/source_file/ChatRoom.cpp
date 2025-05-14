#include "../header_file/Chatroom.h"
#include "../header_file/ChatServer.h"
#include "../header_file/MessagePacket.h"
#include <map>
#include <string>
#include <vector>

extern std::mutex cout_mutex;

void ChatRoom::broadcast(MessagePacket &message) {
    for (auto user : users) {
        if (user->isConnected) {
            ChatServer::sendToClient(user->socket, message);
        }
    }
}

void ChatRoom::getRoomFeatures() {
    std::map<std::string, int> msgCount;
    for (auto msg : messages) {
        msgCount[msg.sender]++;
    }
    features.clear();
    size_t totMsg = 0;
    for (auto user : users) {
        if (msgCount.find(user->username) == msgCount.end())
            msgCount[user->username] = 0;
        totMsg += msgCount[user->username];
        if (features.empty())
            features.resize(user->features.size());
        for (int i = 0; i < features.size(); i++)
            features[i] += user->features[i] * msgCount[user->username];
    }
    for (int i = 0; i < features.size(); i++) {
        features[i] /= totMsg;
    }
}