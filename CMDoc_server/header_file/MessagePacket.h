#pragma once
#include <cstring>
#include <ctime>
#include <string>

struct MessagePacket {
    char sender[100];
    char content[1024];
    time_t timestamp;

    MessagePacket() {
        memset(sender, 0, sizeof(sender));
        memset(content, 0, sizeof(content));
        timestamp = time(nullptr);
    }

    void set(const std::string &_sender, const std::string &_content) {
        strncpy(sender, _sender.c_str(), sizeof(sender) - 1);
        strncpy(content, _content.c_str(), sizeof(content) - 1);
        timestamp = time(nullptr);
    }

    // Serialize to byte array
    std::string serialize() const {
        std::string data;
        data.append(reinterpret_cast<const char *>(this), sizeof(MessagePacket));
        return data;
    }

    // Deserialize from byte array
    static MessagePacket deserialize(const char *buffer, int length) {
        MessagePacket packet;
        if (length >= sizeof(MessagePacket)) {
            memcpy(&packet, buffer, sizeof(MessagePacket));
        } else {
            memset(&packet, 0, sizeof(MessagePacket)); // Clear the packet if the buffer is too small
        }
        return packet;
    }
};
