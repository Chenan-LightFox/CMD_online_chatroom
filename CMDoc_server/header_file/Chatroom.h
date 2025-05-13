#pragma once

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <mutex>
#include "User.h"

class ChatRoom {
public:
    std::string roomName;
    std::set<std::string> topicTags;
    std::vector<User*> users;

    ChatRoom(std::string name, std::set<std::string> tags) : roomName(name), topicTags(tags) {}

    void addUser(User* user) {
        users.push_back(user);
    }

    void broadcast(const std::string& sender, const std::string& message);
};