#pragma once
#include <cstring>
#include <ctime>
#include <string.h>
#include <string>

struct MessagePacket {
    char sender[128];
    char content[1024];
    time_t timestamp;

    MessagePacket(const std::string &_sender="", const std::string &_content="") {
        strcpy_s(sender,_sender.c_str());
        strcpy_s(content,_content.c_str());
        timestamp = time(nullptr);
    }
};
