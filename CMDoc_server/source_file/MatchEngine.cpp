#include "../header_file/MatchEngine.h"
#include <codecvt>
#include <functional>
#include <locale>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <windows.h>

std::wstring string2wstring(std::string str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

Tokenizer::Tokenizer(std::vector<std::string> &dictVec) {
    maxWordLen = 0;
    for (auto word : dictVec) {
        std::wstring wWord = string2wstring(word);
        dict.insert(wWord);
        maxWordLen = max(maxWordLen, wWord.length());
    }
}
std::vector<std::wstring> Tokenizer::fmmTokenizer(const std::wstring &str) {
    std::vector<std::wstring> tokens;
    size_t pos = 0;
    while (pos < str.size()) {
        int len = min(str.size() - pos, maxWordLen);
        std::wstring word;
        while (len > 0) {
            std::wstring sub = str.substr(pos, len);
            if (dict.count(sub)) {
                word = sub;
                break;
            }
            len--;
        }
        if (!word.empty()) {
            tokens.push_back(word);
            pos += word.size();
        } else {
            tokens.push_back(str.substr(pos, 1)); // 单字切分
            pos++;
        }
    }
    return tokens;
}
std::map<std::string, double>
FeatureExtractor::extractFeatures(const std::vector<MessagePacket> &chats) {

    std::map<std::string, double> features;
    size_t totalLength = 0, totalPunctuations = 0, totalWords = 0;
    std::map<std::wstring, int> wordFreq;

    for (const auto &line : chats) {
        auto wLine = string2wstring(line.content);
        totalLength += wLine.length();
        auto tokens = tokenizer.fmmTokenizer(wLine);
        for (const auto &t : tokens) {
            if (t.length() > 1) {
                wordFreq[t]++;
                totalWords++;
            }
        }
        for (wchar_t c : wLine) {
            if (ispunct(c))
                totalPunctuations++;
        }
    }

    // 关键词频率
    for (int i = 0; i < topFreqWords.size(); i++) {
        auto word = topFreqWords[i];
        if (wordFreq.find(word) != wordFreq.end())
            features["top" + std::to_string(i) + "FreqWord"] =
                (double)wordFreq.at(word) / totalWords;
        else
            features["top" + std::to_string(i) + "FreqWord"] = 0;
    }

    features["avgMessageLength"] = (double)totalLength / chats.size();
    features["avgMessageLength"] = (double)totalLength / chats.size();
    features["punctuationRate"] = (double)totalPunctuations / chats.size();

    return features;
}

void FeatureExtractor::getTopFreq(int n,
                                  const std::vector<std::string> &chats) {
    std::vector<std::wstring> wChats;
    std::map<std::wstring, int> wordFreq;
    for (auto chat : chats)
        wChats.push_back(string2wstring(chat));
    for (auto &line : wChats) {
        auto tokens = tokenizer.fmmTokenizer(line);
        for (auto t : tokens) {
            if (t.length() > 1)
                wordFreq[t]++;
        }
    }
    std::priority_queue<std::pair<int, std::wstring>,
                        std::vector<std::pair<int, std::wstring>>,
                        std::greater<std::pair<int, std::wstring>>>
        topFreq;
    for (auto it = wordFreq.begin(); it != wordFreq.end(); it++) {
        if (topFreq.size() < n)
            topFreq.push({it->second, it->first});
        else {
            std::pair<int, std::wstring> tmp = {it->second, it->first};
            if (topFreq.top() < tmp) {
                topFreq.pop();
                topFreq.push(tmp);
            }
        }
    }
    topFreqWords.clear();
    while (!topFreq.empty()) {
        topFreqWords.push_back(topFreq.top().second);
        topFreq.pop();
    }
    return;
}

void Normalizer::normalize(
    std::vector<std::map<std::string, double>> &featureMaps,
    const std::vector<std::string> &allFeatures) {
    std::map<std::string, double> min_val, max_val;

    for (const auto &f : allFeatures) {
        min_val[f] = 1e9;
        max_val[f] = -1e9;
        for (const auto &vec : featureMaps) {
            min_val[f] = min(min_val[f], vec.at(f));
            max_val[f] = max(max_val[f], vec.at(f));
        }
    }

    for (auto &vec : featureMaps) { // 标准化
        for (const auto &f : allFeatures) {
            if (max_val[f] != min_val[f]) {
                vec[f] = (vec[f] - min_val[f]) / (max_val[f] - min_val[f]);
            } else {
                vec[f] = 0.0;
            }
        }
    }
}

std::vector<double>
Normalizer::toVector(const std::map<std::string, double> &featureMap,
                     const std::vector<std::string> &allFeatures) {

    std::vector<double> v;
    for (const auto &f : allFeatures) {
        v.push_back(featureMap.at(f));
    }
    return v;
}
double Similarity::cosineSimilarity(const std::vector<double> &v1,
                                    const std::vector<double> &v2) {
    double dot = 0.0, norm1 = 0.0, norm2 = 0.0;
    for (size_t i = 0; i < v1.size(); ++i) {
        dot += v1[i] * v2[i];
        norm1 += v1[i] * v1[i];
        norm2 += v2[i] * v2[i];
    }
    return (norm1 && norm2) ? dot / (sqrt(norm1) * sqrt(norm2)) : 0.0;
}