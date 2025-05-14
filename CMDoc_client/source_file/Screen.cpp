#include "../header_file/Screen.h"
#include "../header_file/PrintLog.h"
#include <conio.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <minwindef.h>
#include <mutex>
#include <rpcndr.h>
#include <string>
#include <synchapi.h>
#include <winuser.h>

Screen::Screen(std::string userName, short width, short height,
               double messageHeight)
    : width(width), height(height), messageHeight(messageHeight),
      userName(userName), bufMsg(0) {
    system(((std::string)("mode con cols=" + std::to_string(width) +
                          " lines=" + std::to_string(height)))
               .c_str());
    // 获取默认标准显示缓冲区句柄
    coord = {0, 0};
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    // 创建新的缓冲区
    hOutBuf = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    // 设置新的缓冲区为活动显示缓冲
    SetConsoleActiveScreenBuffer(hOutBuf);

    // 隐藏两个缓冲区的光标
    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0;
    cci.dwSize = 1;
    SetConsoleCursorInfo(hOutput, &cci);
    SetConsoleCursorInfo(hOutBuf, &cci);
}
void Screen::draw() {
    DWORD bytes = 0;
    char *data = new char[height * width];
    memset(data, 0, height * width);
    int lastSize = 0;
    int lastBuf = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(messageMutex);
        if (lastSize != messageStack.size() || lastBuf != bufMsg) {
            lastBuf = bufMsg;
            std::deque<std::pair<int, MessagePacket>> showQueue;
            for (int i = 0; i < bufMsg && !messageStack.empty(); i++) {
                messageStackBuf.push(messageStack.top());
                messageStack.pop();
            }

            int lineCount = 0;
            while (!messageStack.empty()) {
                auto msg = messageStack.top();
                MessagePacket newMsg(msg.sender);
                newMsg.timestamp = msg.timestamp;
                messageStack.pop();
                messageStackBuf.push(msg);

                int line = 0, off = 0;
                int j = 0, cnt = 0;
                for (j = 0; msg.content[j] != '\0'; j++) {
                    cnt++;
                    if (cnt >= (width - 1) && msg.content[j] > 0) {
                        newMsg.content[j + off] = '\n';
                        off++;
                        cnt = 0;
                    }
                    newMsg.content[j + off] = msg.content[j];
                    if (msg.content[j] == '\n') {
                        cnt = 0;
                        line++;
                    }
                }
                newMsg.content[j + off] = '\0';
                line += 3 + off;
                lineCount += line;
                showQueue.push_back({line, newMsg});
                if (lineCount > height * messageHeight) {
                    showQueue.pop_back();
                    lineCount -= line;
                    messageStack.push(messageStackBuf.top());
                    messageStackBuf.pop();
                    break;
                }
            }
            while (!messageStackBuf.empty()) {
                messageStack.push(messageStackBuf.top());
                messageStackBuf.pop();
            }
            lastSize = messageStack.size();
            lock.unlock();
            std::lock_guard<std::mutex> lock2(coutMutex);

            DWORD consoleSize = height * width;
            FillConsoleOutputCharacter(hOutput, ' ', consoleSize, {0, 0},
                                       &bytes);
            SetConsoleCursorPosition(hOutput, {0, 0});
            while (!showQueue.empty()) {
                auto msg = showQueue.back().second;
                showQueue.pop_back();
                tm localtime;
                localtime_s(&localtime, &msg.timestamp);
                std::string timestr = std::to_string(localtime.tm_hour) + ":" +
                                      std::to_string(localtime.tm_min) + ":" +
                                      std::to_string(localtime.tm_sec);
                if (userName == msg.sender) {
                    std::cout << std::right << std::setw(width)
                              << (timestr + ": " + "<" + msg.sender + ">")
                              << "\n";
                    std::cout << std::right << std::setw(width) << msg.content
                              << "\n\n";
                } else {
                    std::cout << timestr << ": ";
                    std::cout << "<" << msg.sender << "> \n"
                              << msg.content << "\n\n";
                }
            }
            SetConsoleCursorPosition(hOutput,
                                     {0, (short)(height * messageHeight)});
            for (int i = 0; i < width; i++)
                std::cout << '-';
            std::cout << '\n';

            ReadConsoleOutputCharacterA(hOutput, data, height * width, coord,
                                        &bytes);
            WriteConsoleOutputCharacterA(hOutBuf, data, height * width, coord,
                                         &bytes);
            SetConsoleCursorPosition(hOutBuf,
                                     {0, (short)(height * messageHeight + 1)});
        }
        Sleep(10);
    }
}
void Screen::setBufMsg() { int ch; }