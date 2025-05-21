#pragma once
#include "MessagePacket.h"
#include <mutex>
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
    int bufMsg;
    short width;
    short height;
    double messageHeight;

    std::string userName;

  public:
    Screen(const std::string &userName, short width, short height,
           double messageHeight = 0.7);
    void draw();
    inline const int getBufMsg() const { return bufMsg; }
    inline void setBufMsg(int new_bufMsg) { bufMsg = new_bufMsg; }
    inline void decrementBufMsg() {
        if (bufMsg > 0) {
            bufMsg--;
        }
    }
    inline void incrementBufMsg() { bufMsg++; }
};