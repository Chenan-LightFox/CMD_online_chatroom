#pragma once

#include "MessagePacket.h"
#include <string>
#include <fstream>
#include <ctime>

class ChatHistoryManager {
public:
    static void saveMessage(const std::string &roomName, const MessagePacket &message);
};
