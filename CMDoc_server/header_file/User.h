#pragma once

#include <string>
#include <set>
#include "../header_file/Chatroom.h"

class User {
public:
    std::string username;
    std::string password;
    std::set<std::string> interests;
    ChatRoom* joinedRoom = nullptr;
    int socket = -1;

    User(std::string uname, std::string pwd) : username(uname), password(pwd) {}

    void updateInterests(const std::set<std::string>& newInterests) {
        interests = newInterests;
    }
};S