#pragma once

#include "Chatroom.h"
#include "MessagePacket.h"
#include <mutex>
#include <queue>
#include <string>

class ChatHistoryManager {
  public:
    static void saveHistory(ChatRoom *room);
    static void loadHistory(ChatRoom *room,
                            std::map<std::string, User *> &registeredUsers);
};
