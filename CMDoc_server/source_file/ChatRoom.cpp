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

bool ChatRoom::roomExists(const std::string &name) {
    std::lock_guard<std::mutex> lock(roomMutex);
    for (auto room : rooms) {
        if (room->roomName == name)
            return true;
    }
    return false;
}

bool ChatRoom::createRoom(const std::string &name) {
    std::lock_guard<std::mutex> lock(roomMutex);
    if (roomExists(name))
        return false;
    rooms.push_back(new ChatRoom(name));
    return true;
}
/*
void ChatRoom::listRooms() {
    for (auto room : rooms) {
        //
    }
}

void ChatRoom::getRoomMembers(const std::string name){
    //
}
*/
std::vector<std::string> ChatRoom::getRoomNames() {
    std::lock_guard<std::mutex> lock(roomMutex);
    std::vector<std::string> names;
    for (auto room : rooms)
        names.push_back(room->roomName);
    return names;
}