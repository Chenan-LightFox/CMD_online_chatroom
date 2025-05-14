#pragma once

#include "MessagePacket.h"
#include "User.h"
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <vector>

class ChatRoom {
  public:
    std::string roomName;
    std::set<User *> users;
    std::vector<double> features;

    ChatRoom(std::string name) : roomName(name) {}

    void addUser(User *user) { users.insert(user); }

    void broadcast(MessagePacket &message);
};
