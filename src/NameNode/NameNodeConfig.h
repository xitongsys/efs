#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "GroupDesc.h"
#include "HostDesc.h"
#include "UserDesc.h"

namespace efs {
class NameNodeConfig {
public:
    std::string ip;
    int16_t port;

    std::unordered_set<std::string> tokens;

    //["user01,pwd01,1,1,/user01", "user02,pwd02,2,1,/user02"]
    std::vector<UserDesc> users;

    //["group01,1"]
    std::vector<GroupDesc> groups;

    std::string log_path;
};
}