#pragma once

#include "MessagePacket.h"
#include <mutex>
#include <queue>
#include <string>


class ChatHistoryManager {
  public:
    static void saveHistory(const std::string &roomName);
    static void loadHistory(const std::string &roomName);
};
