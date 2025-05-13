#pragma once

#include "User.h"
#include "PrintLog.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>

class UserDataManager {
public:
    bool loadUsers(const std::string &filename, std::map<std::string, User *> &users);
    bool saveUsers(const std::string &filename, const std::map<std::string, User *> &users);
};
