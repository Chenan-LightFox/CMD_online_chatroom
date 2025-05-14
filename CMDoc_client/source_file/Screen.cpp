#include "../header_file/Screen.h"
#include "../header_file/PrintLog.h"
#include <cstring>
#include <iostream>
#include <minwindef.h>
#include <mutex>
#include <queue>
#include <rpcndr.h>
#include <string>
#include <synchapi.h>
Screen::Screen(short width, short height, double messageHeight)
    : width(width), height(height), messageHeight(messageHeight) {
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
    char data[3200];
    memset(data, 0, sizeof(data));
    int lastSize = 0;

    while (true) {
        if (lastSize != messageQueue.size()) {
            std::unique_lock<std::mutex> lock(messageMutex);
            std::queue<std::pair<int, MessagePacket>> showQueue;
            int msgCount = messageQueue.size();
            int lineCount = 0;
            for (int i = 0; i < msgCount; i++) {
                auto msg = messageQueue.front();
                MessagePacket newMsg(msg.sender);
                newMsg.timestamp = msg.timestamp;
                messageQueue.pop();

                int line = 0, off = 0;
                int j = 0, cnt = 0;
                for (j = 0; msg.content[j] != '\0'; j++) {
                    cnt++;
                    if (cnt == (width - 1)) {
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
                showQueue.push({line, newMsg});
                messageQueue.push(msg);
            }
            while (lineCount > height * messageHeight && showQueue.size() > 1) {
                lineCount -= showQueue.front().first;
                showQueue.pop();
                messageQueue.pop();
            }
            lastSize = messageQueue.size();
            lock.unlock();
            std::lock_guard<std::mutex> lock2(coutMutex);

            DWORD consoleSize = height * width;
            FillConsoleOutputCharacter(hOutput, ' ', consoleSize, {0, 0},
                                       &bytes);
            SetConsoleCursorPosition(hOutput, {0, 0});
            while (!showQueue.empty()) {
                auto msg = showQueue.front().second;
                showQueue.pop();
                tm localtime;
                localtime_s(&localtime, &msg.timestamp);
                std::string timestr = std::to_string(localtime.tm_hour) + ":" +
                                      std::to_string(localtime.tm_min) + ":" +
                                      std::to_string(localtime.tm_sec);
                std::cout << timestr << ": ";
                std::cout << "<" << msg.sender << "> \n"
                          << msg.content << "\n\n";
            }
            SetConsoleCursorPosition(hOutput,
                                     {0, (short)(height * messageHeight)});
            for (int i = 0; i < width; i++)
                std::cout << '-';
            std::cout << '\n';

            ReadConsoleOutputCharacterA(hOutput, data, sizeof(data), coord,
                                        &bytes);
            WriteConsoleOutputCharacterA(hOutBuf, data, sizeof(data), coord,
                                         &bytes);
            SetConsoleCursorPosition(hOutBuf,
                                     {0, (short)(height * messageHeight + 1)});
        }
        Sleep(10);
    }
}