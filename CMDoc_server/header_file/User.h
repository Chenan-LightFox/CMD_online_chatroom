#pragma once

#include "MessagePacket.h"
#include <set>
#include <map>
#include <string>
#include <vector>
#include <winsock2.h>

class User {
  public:
    std::string username;
    std::string password;
    std::map<std::string, double> interestProfile;
    std::vector<MessagePacket> recentMessages;
    int joinedRoom = 0;
    SOCKET socket;
    bool isConnected;
    User(std::string uname, std::string pwd)
        : username(uname), password(pwd), isConnected(true) {}
};