#include "../header_file/ChatClient.h"
#include <cstring>
#include <iostream>
#include <string.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
int main() {
    ChatClient client("127.0.0.1", 8080);
    client.start();
    std::string message;
    while (std::getline(std::cin, message)) {
        MessagePacket package;
        strcpy_s(package.sender, message.c_str());
        client.test(package);
    }
    return 0;
}