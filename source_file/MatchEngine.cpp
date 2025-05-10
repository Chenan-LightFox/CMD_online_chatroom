#include "../header_file/MatchEngine.h"

std::vector<ChatRoom*> rooms;

ChatRoom* MatchEngine::matchUserToRoom(User* user) {
    for (auto room : rooms) {
        for (const auto& tag : user->interests) {
            if (room->topicTags.count(tag)) {
                return room;
            }
        }
    }
    return nullptr;
}