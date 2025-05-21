#pragma once

#include "../header_file/ChatServer.h"
#include <string>

class ServerCommand {
  public:
    virtual void execute(const std::string &cmd, ChatServer &server) = 0;
};

class HelpCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer &server) override;
};

class RoomCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer &server) override;
};

class StopCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer &server) override;
};