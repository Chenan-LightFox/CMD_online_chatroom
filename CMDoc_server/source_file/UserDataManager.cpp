#include "../header_file/UserDataManager.h"

bool UserDataManager::loadUsers(const std::string &filename, std::map<std::string, User *> &users) {
    std::ifstream inFile(filename);
    if (!inFile.is_open())
        return false;

    std::string username, password;
    int interestCount;
    while (inFile >> username >> password >> interestCount) {
        User *user = new User(username, password);
        for (int i = 0; i < interestCount; ++i) {
            std::string tag;
            double weight;
            inFile >> tag >> weight;
            user->interestProfile[tag] = weight;
        }
        users[username] = user;
    }

    inFile.close();
    return true;
}

bool UserDataManager::saveUsers(const std::string &filename, const std::map<std::string, User *> &users) {
    std::ofstream outFile(filename);
    if (!outFile.is_open())
        return false;

    for (const auto &pair : users) {
        User *user = pair.second;
        outFile << user->password << "," << user->interestProfile.size();
        for (const auto &pair : user->interestProfile) {
            outFile << pair.first << "," << pair.second << "\n";
        }
        outFile << "\n";
    }

    outFile.close();
    return true;
}