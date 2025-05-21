#pragma once

#include "MessagePacket.h"
#include <mutex>
#include <string>
#include <vector>
#include <winsock2.h>

class User {
  public:
    std::string username;
    std::string password;
    SOCKET socket;
    std::vector<MessagePacket> recentMessages;
    std::vector<double> features;
    std::mutex featureMutex;
    int joinedRoom = 0;
    bool isConnected;
    User(const std::string &uname, const std::string &pwd)
        : username(uname), password(pwd) {}
};