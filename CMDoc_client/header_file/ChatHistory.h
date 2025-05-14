#include "MessagePacket.h"
#include <mutex>
#include <queue>
#include <string>
extern std::queue<MessagePacket> messageQueue;
extern std::mutex messageMutex;
class ChatHistory {
  public:
    static void saveHistory(const std::string &fileName);
    static void loadHistory(const std::string &fileName);
};