#include <iostream>
#include <yaml-cpp/yaml.h>

#include "Error.h"
#include "FS.h"
#include "GroupDesc.h"
#include "NameNodeConfig.h"
#include "UserDesc.h"
#include "Util.h"

namespace efs {

NameNodeConfig::NameNodeConfig(const std::string& file)
{
    YAML::Node node = YAML::LoadFile(file);

    ip = node["ip"].as<std::string>();
    port = node["port"].as<uint16_t>();

    for (auto& token : node["tokens"].as<std::vector<std::string>>()) {
        tokens.insert(token);
    }

    auto users_str = node["users"].as<std::vector<std::string>>();
    for (auto& us : users_str) {
        UserDesc udesc(us);
        users.push_back(udesc);
    }

    auto groups_str = node["groups"].as<std::vector<std::string>>();
    for (auto& gs : groups_str) {
        GroupDesc gdesc(gs);
        groups.push_back(gdesc);
    }

    log_path = node["log_path"].as<std::string>();
}

NameNodeConfig::NameNodeConfig(const NameNodeConfig& config)
{
    ip = config.ip;
    port = config.port;
    tokens = config.tokens;
    users = config.users;
    groups = config.groups;
    log_path = config.log_path;
}

}
