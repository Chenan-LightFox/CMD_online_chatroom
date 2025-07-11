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

void ChatRoom::getRoomFeatures(int featureSize, std::vector<User *> &users) {
    std::map<std::string, int> msgCount;
    for (auto msg : messages) {
        msgCount[msg.sender]++;
    }
    features.clear();
    features = std::vector<double>(featureSize, 0);
    size_t totMsg = 0;
    for (auto user : users) {
        if (msgCount.find(user->username) == msgCount.end())
            msgCount[user->username] = 0;
        totMsg += msgCount[user->username];
        for (int i = 0; i < features.size(); i++)
            features[i] += user->features[i] * msgCount[user->username];
    }
    for (double &feature : features) {
        feature /= totMsg;
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
    if (~roomExists(name))
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
        result += " > " + room->roomName + "\n";
    }
    return result;
}

std::string ChatRoom::getRoomMembers(const std::string &name) {
    std::lock_guard<std::mutex> lock(roomMutex);
    int index = roomExists(name);
    if (index == -1) {
        return "Room \"" + name + "\" does not exist.";
    }
    ChatRoom *targetRoom = rooms[index];
    std::string result = "Now the room \"" + name + "\" has " +
                         std::to_string(targetRoom->users.size()) +
                         " member(s):\n";

    for (auto user : targetRoom->users) {
        result += " > " + user->username + "\n";
    }

    return result;
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
        createRoom("Lobby");
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
    names.reserve(rooms.size());
    for (auto room : rooms)
        names.push_back(room->roomName);
    return names;
}