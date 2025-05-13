#pragma once

#include "Chatroom.h"
#include <map>
#include <set>
#include <string>
#include <vector>

class MatchEngine {
  public:
    ChatRoom *matchUserToRoom(User *user);
};

class Tokenizer {
  private:
    std::set<std::string> dict;

  public:
    std::vector<std::string> fmmTokenizer(const std::string &str);
};

class FeatureExtractor {
  private:
  public:
    static std::map<std::string, std::vector<double>>
    extractFeatures(const std::vector<std::string> &chats,
                    const std::vector<std::string> &keywords);
};

class Normalizer {};

class Similarity {};