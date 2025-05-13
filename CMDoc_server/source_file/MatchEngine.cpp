#include "../header_file/MatchEngine.h"
/*
std::vector<ChatRoom*> rooms;

MatchEngine::MatchEngine() {
    rooms.push_back(new ChatRoom("Lobby", {})); 
    rooms.push_back(new ChatRoom("Game", {"game", "rpg", "fps", "stg"}));
}

ChatRoom* MatchEngine::matchUserToRoom(User* user) {
    // Try to match user with a room
    for (auto room : rooms) {
        for (const auto& tag : user->interests) {
            if (room->topicTags.count(tag)) {
                room->addUser(user);
                user->joinedRoom = room;
                return room;
            }
        }
    }

    // If user does not match any room, add them to the lobby
    rooms[0]->addUser(user);
    user->joinedRoom = rooms[0];
    return rooms[0];
}
*/