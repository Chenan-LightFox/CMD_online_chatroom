#include "../header_file/ChatServer.h"
#include "../header_file/CommandManager.h"
#include "../header_file/PrintLog.h"
#include "../header_file/ServerCommand.h"


#pragma comment(lib, "ws2_32.lib")
std::mutex cout_mutex;

int main() {
    ChatServer server(8088); // todo: read port from config file
    std::thread serverThread([&server]() { server.start(); });

    // Register commands
    CommandManager cmdMgr;
    // cmdMgr.registerCommand("features", std::make_unique<FeaturesCommand>());
    cmdMgr.registerCommand("help", std::make_unique<HelpCommand>());
    cmdMgr.registerCommand("room", std::make_unique<RoomCommand>());
    cmdMgr.registerCommand("stop", std::make_unique<StopCommand>());

    // Get command
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
        if (cmd.empty())
            continue;
        cmdMgr.executeCommand(cmd, server);
        /* if (cmd0 == "stop") {
            server.stop();
            break;
        } else if (cmd0 == "help") {
            printInfo("Available commands: stop, help");
        } else if (cmd0 == "features") {
            server.getFeatures();
        } else if (cmd0 == "room") {
            std::string cmd1;
            std::getline(std::cin, cmd1);

        } else {
            printWarning("Unknown command: " + cmd +
                         ". Type 'help' for available commands.");
        }*/
    }

    if (serverThread.joinable())
        serverThread.join();
    return 0;
}