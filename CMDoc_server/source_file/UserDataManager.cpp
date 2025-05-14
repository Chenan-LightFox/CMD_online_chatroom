#include "../header_file/UserDataManager.h"
#include <filesystem>

namespace fs = std::filesystem; // Only available in C++17 and later

bool UserDataManager::loadUsers(const std::string &directory, std::map<std::string, User *> &users) {
    if (!fs::exists(directory))
        fs::create_directory(directory);

    for (const auto &entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) { // Filter for regular files
            std::ifstream inFile(entry.path());
            if (!inFile.is_open())
                continue;

            std::string username, password;
            int interestCount;
            inFile >> username >> password >> interestCount;

            User *user = new User(username, password);
            for (int i = 0; i < interestCount; ++i) {
                std::string tag;
                double weight;
                inFile >> tag >> weight;
                user->interestProfile[tag] = weight;
            }
            users[username] = user;
            inFile.close();
        }
    }

    return true;
}

bool UserDataManager::saveUsers(const std::string &directory, const User *user) {
    if (!fs::exists(directory))
        fs::create_directory(directory);

    std::string path = directory + "/" + user->username + ".dat";
    std::ofstream outFile(path);
    if (!outFile.is_open())
        return false;

    outFile << user->username << " " << user->password << " "
            << user->interestProfile.size();
    for (const auto &pair : user->interestProfile) {
        outFile << pair.first << " " << pair.second << "\n";
    }
    outFile << "\n";
    outFile.close();
    return true;
}