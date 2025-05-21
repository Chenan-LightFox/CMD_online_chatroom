#include "../header_file/ChatClient.h"
#include "../header_file/ChatHistory.h"
#include "../header_file/PrintLog.h"
#include "../header_file/Screen.h"
#include <conio.h>
#include <iostream>
#include <string>
#include <thread>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

std::mutex coutMutex;
std::mutex messageMutex;

int main() {

    ChatHistory::loadHistory("history.dat");

    std::string userName, passWord;
    bool isRegistered = true;
    std::cout << "\n\n\n\t\t\tEnter Username(Enter 'Register' for register):";
    std::cin >> userName;
    if (userName == "Register") {
        isRegistered = false;
        std::cout << "\n\t\t\tEnter Username for register:";
        std::cin >> userName;
    }
    std::cout << "\n\t\t\tEnter Password:";
    std::cin >> passWord;

    ChatClient client("127.0.0.1", 8088, userName);
    std::thread saveThread(ChatHistory::saveHistory, "history.dat");
    saveThread.detach();

    Screen screen(userName, 120, 30);
    std::thread screenThread(&Screen::draw, &screen);
    screenThread.detach();

    client.start();
    if (isRegistered)
        client.sendPackage(
            {userName, ("/login " + userName + " " + passWord).c_str()});
    else
        client.sendPackage(
            {userName, ("/register " + userName + " " + passWord).c_str()});

    std::string message;
    while (std::getline(std::cin, message)) {
        if (message == "/roll") {
            char ch;
            while (true) {
                if (_kbhit()) {
                    ch = _getch();
                    if (ch == 72)
                        screen.bufMsg++;
                    if (ch == 80 && screen.bufMsg > 0)
                        screen.bufMsg--;
                    if (ch == 27)
                        break;
                }
                Sleep(50);
            }
            screen.bufMsg = 0;
            continue;
        }
        screen.bufMsg = 0;
        MessagePacket package{userName, message};
        client.sendPackage(package);
    }
    client.stop();
    return 0;
}