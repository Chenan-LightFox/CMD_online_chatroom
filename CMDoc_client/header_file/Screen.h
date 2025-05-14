#pragma once
#include "MessagePacket.h"
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <windows.h>

extern std::mutex coutMutex;
extern std::mutex messageMutex;
extern std::stack<MessagePacket> messageStack;
extern std::stack<MessagePacket> messageStackBuf;

class Screen {
  private:
    HANDLE hOutput;
    HANDLE hOutBuf;
    COORD coord;
    short width;
    short height;
    double messageHeight;

    std::string userName;

  public:
    int bufMsg;
    Screen(std::string userName, short width, short height,
           double messageHeight = 0.7);
    void draw();
    void setBufMsg();
};