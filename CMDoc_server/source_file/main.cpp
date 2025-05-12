#include "../header_file/ChatServer.h"
#include "../header_file/Chatroom.h"
#include "../header_file/MatchEngine.h"
#include "../header_file/PrintLog.h"
#include "../header_file/User.h"


#pragma comment(lib, "ws2_32.lib")
std::mutex cout_mutex;

int main() {
    ChatServer server(8080); // todo: read port from config file
    std::thread serverThread([&server]() { server.start(); });

    // Get command
    std::string cmd;
    while (true) {
        std::getline(std::cin, cmd);
        if (cmd == "stop") {
            server.stop();
            break;
        } else if (cmd == "help") {
            PrintInfo("Available commands: stop, help");
        } else {
            PrintWarning("Unknown command: " + cmd +
                         ". Type 'help' for available commands.");
        }
    }

    if (serverThread.joinable())
        serverThread.join();
    return 0;
}