#include "../header_file/Screen.h"
#include <iostream>
#include <mutex>
#include <queue>
#include <rpcndr.h>
#include <string>
#include <synchapi.h>
Screen::Screen(short width, short height, double messageHeight)
    : width(width), height(height), messageHeight(messageHeight) {
    // 获取默认标准显示缓冲区句柄
    coord = {0, 0};
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT wrt = {0, 0, (short)(width - 1), (short)(height - 1)};
    SetConsoleWindowInfo(hOutput, TRUE, &wrt);
    SetConsoleScreenBufferSize(hOutput, {width, height});

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

    // 双缓冲处理显示
    DWORD bytes = 0;
    char data[800];
    while (1) {
        for (char c = 'a'; c < 'z'; c++) {
            system("cls");
            for (int i = 0; i < 800; i++) {
                printf("%c", c);
            }
            ReadConsoleOutputCharacterA(hOutput, data, 800, coord, &bytes);
            WriteConsoleOutputCharacterA(hOutBuf, data, 800, coord, &bytes);
        }
    }
}
void Screen::draw() {
    DWORD bytes = 0;
    char data[3200];
    int lastSize = messageQueue.size();
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

                int line = 0;
                for (int i = 0; msg.content[i] != '\0'; i++) {
                    if (i % (width - 1) == 0) {
                        newMsg.content[i + line] = '\n';
                        line++;
                    }
                    newMsg.content[i + line] = msg.content[i];
                }
                line += 3;
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

            SetConsoleCursorPosition(hOutput, {0, 0});
            std::lock_guard<std::mutex> lock2(coutMutex);
            while (!showQueue.empty()) {
                auto msg = showQueue.front().second;
                tm localtime;
                localtime_s(&localtime, &msg.timestamp);
                std::string timestr = std::to_string(localtime.tm_hour) + ":" +
                                      std::to_string(localtime.tm_min) + ":" +
                                      std::to_string(localtime.tm_sec);
                std::cout << timestr << ": ";
                std::cout << "<" << msg.sender << "> \n"
                          << msg.content << std::endl;
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
        }
        Sleep(10);
    }
}