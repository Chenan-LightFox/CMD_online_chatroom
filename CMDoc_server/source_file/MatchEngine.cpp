#include "../header_file/MatchEngine.h"
#include "../header_file/PrintLog.h"
#include <cwctype>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <queue>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>
#include <windows.h>

MatchEngine::MatchEngine(std::string dictFileName) {
    std::ifstream dictFile(dictFileName);
    setlocale(LC_ALL, "chs");
    if (!dictFile) {
        throw std::runtime_error("Failed to open dictionary file");
    }

    std::string word;
    while (dictFile >> word) {
        dict.push_back(word);
    }
}

std::wstring string2wstring(const std::string &str) {
    auto wStr = std::filesystem::path(str).wstring();
    return wStr;
}

void MatchEngine::getUsersFeature(std::vector<User *> users) {
    Tokenizer tokenizer(dict);
    FeatureExtractor featureExtractor(tokenizer, 10);
    std::vector<std::string> chats;
    for (auto user : users)
        for (auto message : user->recentMessages)
            chats.push_back(message.content);
    featureExtractor.initTopFreq(chats);
    std::vector<std::map<std::string, double>> featureMaps;
    for (auto user : users) {
        featureMaps.push_back(
            featureExtractor.extractFeatures(user->recentMessages));
    }
    Normalizer::normalize(featureMaps, featureExtractor.allFeatures);
    for (int i = 0; i < users.size(); i++) {
        if (users[i]->recentMessages.empty()) {
            continue;
        }
        std::unique_lock<std::mutex> lock(users[i]->featureMutex);
        users[i]->features =
            Normalizer::toVector(featureMaps[i], featureExtractor.allFeatures);
    }
    return;
}

Tokenizer::Tokenizer(std::vector<std::string> &dictVec) {
    maxWordLen = 0;
    try {
        for (auto word : dictVec) {
            std::wstring wWord = string2wstring(word);
            dict.insert(wWord);
            maxWordLen = max(maxWordLen, wWord.length());
        }
    } catch (const std::exception &e) {
        throw std::runtime_error(
            std::string("[Tokenizer] Failed to initialize: ") + e.what());
    }
}
std::vector<std::wstring> Tokenizer::fmmTokenizer(const std::wstring &str) {
    std::vector<std::wstring> tokens;
    size_t pos = 0;
    while (pos < str.size()) {
        size_t len = min(str.size() - pos, maxWordLen);
        std::wstring word;
        if (str[pos] <= 'Z' && str[pos] >= 'A' ||
            str[pos] <= 'z' && str[pos] >= 'a') { // 英文单词
            std::wstring tok;
            while (str[pos] <= 'Z' && str[pos] >= 'A' ||
                   str[pos] <= 'z' && str[pos] >= 'a') {
                tok += str[pos];
                pos++;
            }
            tokens.push_back(tok);
            continue;
        }
        if (str[pos] < '0' && str[pos] > '9') {
            std::wstring tok;
            while (str[pos] < '0' && str[pos] > '9') {
                tok += str[pos];
                pos++;
            }
            tokens.push_back(tok);
            continue;
        }
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
FeatureExtractor::FeatureExtractor(Tokenizer tokenizer, int n)
    : tokenizer(tokenizer), n(n) {
    const std::string allFeaturesStr[] = {
        "interAvgReplyHour", "interReplyFreq", "vocRichness",    "senLenAvg",
        "senLenVar",         "punRate",        "punQuesMarkRate"};
    for (int i = 0; i < n; i++)
        allFeatures.push_back("vocTop" + std::to_string(i) + "FreqWord");
    for (auto i : allFeaturesStr)
        allFeatures.push_back(i);
}
std::map<std::string, double>
FeatureExtractor::extractFeatures(const std::vector<MessagePacket> &chats) {

    std::map<std::string, double> features;

    interactionLevel(chats, features);
    vocabularyLevel(chats, features);
    sentenceLevel(chats, features);
    punctuationLevel(chats, features);

    return features;
}

void FeatureExtractor::interactionLevel(
    const std::vector<MessagePacket> &chats,
    std::map<std::string, double> &features) {
    time_t avgReplyHour = 0;

    time_t startStamp = 0x3f3f3f3f3f3f3f3f, endStamp = 0;

    for (const auto &msg : chats) {
        tm localtime;
        localtime_s(&localtime, &msg.timestamp);
        avgReplyHour += localtime.tm_hour;
        startStamp = min(startStamp, msg.timestamp);
        endStamp = max(endStamp, msg.timestamp);
    }
    if (endStamp == startStamp)
        endStamp++; // avoid division by zero
    features["interAvgReplyHour"] = (double)avgReplyHour / chats.size();
    features["interReplyFreq"] = (double)chats.size() / (endStamp - startStamp);
}
void FeatureExtractor::vocabularyLevel(
    const std::vector<MessagePacket> &chats,
    std::map<std::string, double> &features) {
    size_t totalLength = 0, totalWords = 0;
    std::map<std::wstring, int> wordFreq;
    std::set<std::wstring> independentWords;

    for (const auto &line : chats) {
        auto wLine = string2wstring(line.content);
        totalLength += wLine.length();
        auto tokens = tokenizer.fmmTokenizer(wLine);
        for (const auto &t : tokens) {
            if (t.length() > 1) {
                wordFreq[t]++;
                totalWords++;
                independentWords.insert(t);
            }
        }
    }

    // 关键词频率
    for (int i = 0; i < topFreqWords.size(); i++) {
        auto word = topFreqWords[i];
        if (wordFreq.find(word) != wordFreq.end())
            features["vocTop" + std::to_string(i) + "FreqWord"] =
                (double)wordFreq.at(word) / totalWords;
        else
            features["vocTop" + std::to_string(i) + "FreqWord"] = 0;
    }
    features["vocRichness"] = (double)independentWords.size() / totalWords;
}
void FeatureExtractor::sentenceLevel(const std::vector<MessagePacket> &chats,
                                     std::map<std::string, double> &features) {
    std::vector<long long> sentenceLength;
    for (const auto &msg : chats) {
        std::wstring wstrMsg = string2wstring(msg.content);
        int cnt = 0;
        for (int i = 0; i < wstrMsg.length(); i++) {
            cnt++;
            if (iswpunct(wstrMsg[i])) {
                if (cnt > 1)
                    sentenceLength.push_back(cnt);
                cnt = 0;
            }
        }
        if (cnt > 1)
            sentenceLength.push_back(cnt);
    }
    double avgSenLen = 0, avgSenSquareLen = 0;
    for (auto len : sentenceLength) {
        avgSenLen += len;
        avgSenSquareLen += len * len;
    }
    avgSenLen /= sentenceLength.size();
    avgSenSquareLen /= sentenceLength.size();

    features["senLenAvg"] = avgSenLen;
    features["senLenVar"] = avgSenSquareLen - (avgSenLen * avgSenLen);
}

void FeatureExtractor::punctuationLevel(
    const std::vector<MessagePacket> &chats,
    std::map<std::string, double> &features) {
    long long totalPunct = 0, totalQuestionMark = 0;
    for (const auto &msg : chats) {

        std::wstring wstrMsg = string2wstring(msg.content);
        for (int i = 0; i < wstrMsg.length(); i++) {
            if (iswpunct(wstrMsg[i])) {
                if (wstrMsg[i] == '?' || wstrMsg[i] == L'?')
                    totalQuestionMark++;
                while (++i < wstrMsg.length() && iswpunct(wstrMsg[i]))
                    ;
                totalPunct++;
            }
        }
    }
    features["punRate"] = (double)totalPunct / chats.size();
    features["punQuesMarkRate"] = (double)totalQuestionMark / chats.size();
}

void FeatureExtractor::initTopFreq(const std::vector<std::string> &chats) {
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
    if (topFreqWords.size() < n) {
        for (int i = topFreqWords.size(); i < n; i++) {
            topFreqWords.push_back(L"");
        }
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