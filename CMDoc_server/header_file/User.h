#pragma once

#include <string>
#include <set>

class User {
public:
    std::string username;
    std::string password;
    std::set<std::string> interests;

    User(std::string uname, std::string pwd) : username(uname), password(pwd) {}

    void updateInterests(const std::set<std::string>& newInterests) {
        interests = newInterests;
    }
};