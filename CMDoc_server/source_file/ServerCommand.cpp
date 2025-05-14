#include "../header_file/ServerCommand.h"
#include "../header_file/Chatroom.h"
#include "../header_file/PrintLog.h"
#include <string>

void FeaturesCommand::execute(const std::string &cmd, ChatServer &server) {
    server.getFeatures();
}

void HelpCommand::execute(const std::string &cmd, ChatServer &server) {
    printInfo("Available commands:\n"
              "help - Show this help\n"
              "room - Operate with rooms\n"
              "stop - Stop this server\n");
}

void RoomCommand::execute(const std::string &cmd, ChatServer &server) {
    std::istringstream iss(cmd);
    std::string command;
    iss >> command;

    if (command == "create") {
        std::string roomName;
        iss >> roomName;
        if (roomName.empty()) {
            printWarning("Room name is required!\n");
            return;
        }
        ChatRoom::createRoom(roomName);
    } else if (command == "list") {
        std::string roomName;
        iss >> roomName;
        if (roomName.empty()) {
            printInfo(ChatRoom::listRooms());
            return;
        }
        ChatRoom::getRoomMembers(roomName);
    } else {
        printWarning("Not enough parameters!\n"
                     "Available commands:\n"
                     "create - Create a new room\n"
                     "list - List all rooms\n");
    }
}

void StopCommand::execute(const std::string &cmd, ChatServer &server) {
    server.stop();
    system("pause");
    exit(0);
}