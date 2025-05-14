#include "../header_file/ChatClient.h"
#include "../header_file/ChatHistory.h"
#include "../header_file/PrintLog.h"
#include "../header_file/Screen.h"
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

std::mutex coutMutex;
std::mutex messageMutex;

int main() {
    ChatHistory::loadHistory("history.dat");

    std::string userName, passWord;
    bool isRegistered = true;
    std::cout << "Enter Username(Enter 'Register' for register):";
    std::cin >> userName;
    if (userName == "Register") {
        isRegistered = false;
        std::cout << "Enter Username for register:";
        std::cin >> userName;
    }
    std::cout << "Enter Password:";
    std::cin >> passWord;
    ChatClient client("127.0.0.1", 8088, userName);
    std::thread saveThread(ChatHistory::saveHistory, "history.dat");
    saveThread.detach();

    Screen screen(userName, 100, 30);
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
        MessagePacket package{userName, message};
        client.sendPackage(package);
    }
    client.stop();
    return 0;
}