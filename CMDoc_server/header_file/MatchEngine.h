#pragma once

#include "MessagePacket.h"
#include <map>
#include <set>
#include <string>
#include <vector>

std::wstring string2wstring(std::string str);

class MatchEngine {
  public:
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
  private:
    std::vector<std::wstring> topFreqWords;
    Tokenizer tokenizer;

  public:
    std::map<std::string, double>
    extractFeatures(const std::vector<MessagePacket> &chats);
    void getTopFreq(int n, const std::vector<std::string> &chats);
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
