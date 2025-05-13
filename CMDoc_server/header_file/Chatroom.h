#pragma once

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <mutex>
#include "MessagePacket.h"
#include "User.h"

class ChatRoom {
public:
    std::string roomName;
    std::set<User*> users;

    ChatRoom(std::string name) : roomName(name) {}

    void addUser(User* user) {
        users.insert(user);
    }

    void broadcast(const MessagePacket& message);
};
