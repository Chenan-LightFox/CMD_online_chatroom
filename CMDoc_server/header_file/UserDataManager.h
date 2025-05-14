#pragma once

#include "User.h"
#include "PrintLog.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>

class UserDataManager {
public:
    bool loadUsers(const std::string &directory, std::map<std::string, User *> &users);
    bool saveUsers(const std::string &directory, const User *user);
};
