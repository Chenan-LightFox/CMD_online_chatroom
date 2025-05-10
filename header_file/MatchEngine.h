#pragma once

#include <vector>
#include "Chatroom.h"

class MatchEngine {
public:
    ChatRoom* matchUserToRoom(User* user);
};