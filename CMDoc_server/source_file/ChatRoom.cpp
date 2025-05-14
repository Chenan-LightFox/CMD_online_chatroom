#include "../header_file/Chatroom.h"
#include "../header_file/ChatServer.h"
#include "../header_file/MessagePacket.h"
#include "../header_file/PrintLog.h"
#include <fstream>
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

int ChatRoom::roomExists(const std::string &name) {
    for (int i = 0; i < rooms.size(); i++) {
        if (rooms[i]->roomName == name)
            return i;
    }
    return -1;
}

bool ChatRoom::createRoom(const std::string &name) {
    std::lock_guard<std::mutex> lock(roomMutex);
    if (!(~roomExists(name)))
        return false;
    rooms.push_back(new ChatRoom(name));
    saveRoomList();
    return true;
}

std::string ChatRoom::listRooms() {
    std::string result;
    result =
        "Now the server has " + std::to_string(rooms.size()) + " room(s): ";
    for (auto room : rooms) {
        result += " > " + room->roomName;
    }
    return result;
}

void ChatRoom::getRoomMembers(const std::string &name) {
    //
}

void ChatRoom::saveRoomList() {
    std::ofstream file("room_list.txt", std::ios::trunc);
    if (!file.is_open()) {
        printError("Failed to open room_list.txt for writing.");
        return;
    }
    for (auto room : rooms) {
        file << room->roomName << "\n";
    }
    file.close();
}

void ChatRoom::loadRoomList() {
    std::ifstream file("room_list.txt");
    if (!file.is_open()) {
        printError("Failed to open room_list.txt for reading.");
        return;
    }
    std::string roomName;
    while (std::getline(file, roomName)) {
        if (!roomName.empty()) {
            createRoom(roomName);
        }
    }
    file.close();
}

std::vector<std::string> ChatRoom::getRoomNames() {
    std::lock_guard<std::mutex> lock(roomMutex);
    std::vector<std::string> names;
    for (auto room : rooms)
        names.push_back(room->roomName);
    return names;
}