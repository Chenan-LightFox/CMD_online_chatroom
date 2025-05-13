#pragma once

#include <vector>
#include "Chatroom.h"

class MatchEngine {
private:
    std::vector<ChatRoom *> rooms;

  public:
    MatchEngine();
    ChatRoom* matchUserToRoom(User* user);
};