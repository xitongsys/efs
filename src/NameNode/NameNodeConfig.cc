#include <iostream>
#include <yaml-cpp/yaml.h>

#include "Error.h"
#include "FS.h"
#include "NameNodeConfig.h"
#include "Util.h"

namespace efs {
NameNodeConfig::NameNodeConfig() { }

NameNodeConfig::NameNodeConfig(const std::string& file)
{
    YAML::Node node = YAML::LoadFile(file);

    ip = node["ip"].as<std::string>();
    port = node["port"].as<uint16_t>();

    tokens = fs::formatPath(node["tokens"].as<std::vector<std::string>>());
    users = node["users"].as<std::vector<std::string>>();
    groups = node["groups"].as<std::vector<std::string>>();

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
