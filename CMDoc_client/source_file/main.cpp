#include "../header_file/ChatClient.h"
#include <cstring>
#include <iostream>
#include <string.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

std::mutex cout_mutex;

int main() {
    ChatClient client("127.0.0.1", 8088);
    client.start();
    std::string message;
    while (std::getline(std::cin, message)) {
        MessagePacket package{"", message};
        client.test(package);
    }
    client.stop();
    return 0;
}