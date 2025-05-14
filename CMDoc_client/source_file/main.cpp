#include "../header_file/ChatClient.h"
#include "../header_file/Screen.h"
#include <cstring>
#include <iostream>
#include <string.h>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

std::mutex coutMutex;
std::mutex messageMutex;

int main() {
    ChatClient client("127.0.0.1", 8088);
    Screen screen(80, 40);
    std::thread screenThread(&Screen::draw, &screen);
    client.start();

    std::string message;
    while (std::getline(std::cin, message)) {
        MessagePacket package{"", message};
        client.test(package);
    }
    client.stop();
    return 0;
}