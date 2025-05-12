#pragma once
#include <ctime>
#include <string>
struct MessagePacket {
    char sender[100];
    char content[1024];
    time_t timestamp;
};