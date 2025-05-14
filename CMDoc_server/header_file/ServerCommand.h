#pragma once

#include "../header_file/ChatServer.h"
#include <string>
#include <sstream>

class ServerCommand {
  public:
    virtual void execute(const std::string &cmd, ChatServer &server) = 0;
};

class FeaturesCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer& server);
};

class HelpCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer &server);
};

class RoomCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer &server);
};

class StopCommand : public ServerCommand {
  public:
    void execute(const std::string &cmd, ChatServer& server);
};