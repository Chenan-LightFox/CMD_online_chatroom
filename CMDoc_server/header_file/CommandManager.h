#pragma once
#include "ServerCommand.h"
#include <iostream>
#include <map>
#include <memory>
#include <string>

class CommandManager {
  private:
    std::map<std::string, std::unique_ptr<ServerCommand>> commands;

  public:
    void registerCommand(const std::string &name,
                         std::unique_ptr<ServerCommand> cmd) {
        commands[name] = std::move(cmd);
    }

    void executeCommand(const std::string &input, ChatServer &server) {
        auto space = input.find(' ');
        std::string cmd = input.substr(0, space);
        std::string args =
            (space == std::string::npos) ? "" : input.substr(space + 1);

        auto it = commands.find(cmd);
        if (it != commands.end()) {
            it->second->execute(args, server);
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }
};
