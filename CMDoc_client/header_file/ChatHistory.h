#include "MessagePacket.h"
#include <mutex>
#include <stack>
#include <string>
extern std::stack<MessagePacket> messageStack;
extern std::stack<MessagePacket> messageStackBuf;
extern std::mutex messageMutex;
class ChatHistory {
  public:
    static void saveHistory(const std::string &fileName);
    static void loadHistory(const std::string &fileName);
};