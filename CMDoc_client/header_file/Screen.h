#pragma once
#include "MessagePacket.h"
#include <mutex>
#include <queue>
#include <windows.h>

extern std::mutex coutMutex;
extern std::mutex messageMutex;
extern std::queue<MessagePacket> messageQueue;
class Screen {
  private:
    HANDLE hOutput;
    HANDLE hOutBuf;
    COORD coord;
    short width;
    short height;
    double messageHeight;

  public:
    Screen(short width, short height, double messageHeight = 0.7);
    void draw();
};