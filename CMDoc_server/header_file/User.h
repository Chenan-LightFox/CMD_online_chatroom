#pragma once

#include "MessagePacket.h"
#include <map>
#include <mutex>
#include <set>
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
    User(std::string uname, std::string pwd)
        : username(uname), password(pwd), isConnected(false) {}
};