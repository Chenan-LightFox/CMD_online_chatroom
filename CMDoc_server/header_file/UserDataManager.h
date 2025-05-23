#pragma once

#include "User.h"
#include <map>
#include <string>

class UserDataManager {
  public:
    bool loadUsers(const std::string &directory,
                   std::map<std::string, User *> &users);
    bool saveUsers(const std::string &directory, const User *user);
};
