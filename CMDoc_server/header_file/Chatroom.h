#pragma once

#include "MessagePacket.h"
#include "User.h"
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <vector>

static std::mutex roomMutex;

class ChatRoom {
  public:
    std::string roomName;
    std::set<User *> users;
    std::vector<double> features;
    std::vector<MessagePacket> messages;
    std::mutex messageMutex;
    ChatRoom(std::string name) : roomName(name) {}
    ChatRoom(const ChatRoom &room) {
        roomName = room.roomName;
        users = room.users;
        features = room.features;
        messages = room.messages;
    }

    void addUser(User *user) { users.insert(user); }

    void broadcast(MessagePacket &message);
    void getRoomFeatures(int featureSize, std::vector<User *> &users);

    static int roomExists(const std::string &name);
    static bool createRoom(const std::string &name);
    static std::string listRooms();
    static std::string getRoomMembers(const std::string &name);
    static void saveRoomList();
    static void loadRoomList();
    static std::vector<std::string> getRoomNames();
};

extern std::vector<ChatRoom *> rooms;