#pragma once

#include "MessagePacket.h"
#include "User.h"
#include <map>
#include <set>
#include <string>
#include <vector>

std::wstring string2wstring(const std::string &str);

class MatchEngine {
  private:
    std::vector<std::string> dict;

  public:
    MatchEngine(const std::string &dictFileName);
    void getUsersFeature(std::vector<User *> users);
};

class Tokenizer {
  private:
    std::set<std::wstring> dict;
    size_t maxWordLen;

  public:
    Tokenizer(std::vector<std::string> &dictVec);
    std::vector<std::wstring> fmmTokenizer(const std::wstring &str);
};

class FeatureExtractor {
    friend class MatchEngine;

  private:
    std::vector<std::wstring> topFreqWords;
    Tokenizer tokenizer;
    std::vector<std::string> allFeatures;
    int n;

  public:
    FeatureExtractor(Tokenizer, int n);
    std::map<std::string, double>
    extractFeatures(const std::vector<MessagePacket> &chats);
    void initTopFreq(const std::vector<std::string> &chats);
    void interactionLevel(const std::vector<MessagePacket> &chats,
                          std::map<std::string, double> &features);
    void vocabularyLevel(const std::vector<MessagePacket> &chats,
                         std::map<std::string, double> &features);
    void sentenceLevel(const std::vector<MessagePacket> &chats,
                       std::map<std::string, double> &features);
    void punctuationLevel(const std::vector<MessagePacket> &chats,
                          std::map<std::string, double> &features);
};

class Normalizer {
  public:
    static void
    normalize(std::vector<std::map<std::string, double>> &featureMaps,
              const std::vector<std::string> &allFeatures);
    static std::vector<double>
    toVector(const std::map<std::string, double> &featureMap,
             const std::vector<std::string> &allFeatures);
};

class Similarity {
  public:
    static double cosineSimilarity(const std::vector<double> &v1,
                                   const std::vector<double> &v2);
};
