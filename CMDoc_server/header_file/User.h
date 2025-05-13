#pragma once

#include <string>
#include <set>

class User {
public:
    std::string username;
    std::string password;
    int joinedRoom = 0;
    int socket = -1;

    User(std::string uname, std::string pwd) : username(uname), password(pwd) {}

};