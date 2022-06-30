#include <iostream>
#include <yaml-cpp/yaml.h>

#include "DataNodeConfig.h"
#include "Error.h"
#include "FS.h"
#include "Util.h"
#include "Limit.h"

namespace efs {
DataNodeConfig::DataNodeConfig() { }

DataNodeConfig::DataNodeConfig(const std::string& file)
{
    YAML::Node node = YAML::LoadFile(file);

    name = node["name"].as<std::string>();
    token = node["token"].as<std::string>();

    ip = node["ip"].as<std::string>();
    port = node["port"].as<uint16_t>();

    index_path = fs::format((node["index_path"]).as<std::string>());
    root_path = fs::format(node["root_path"].as<std::string>());
    log_path = fs::format(node["log_path"].as<std::string>());
    disk_size = node["disk_size"].as<uint64_t>();

    namenode_addr = node["namenode_addr"].as<std::string>();
    namenode_port = node["namenode_port"].as<uint16_t>();

    paths = node["paths"].as<std::vector<std::string>>();
    init_paths = node["init_paths"].as<std::vector<std::string>>();
}

DataNodeConfig::DataNodeConfig(const DataNodeConfig& config)
{
    name = config.name;
    token = config.token;
    ip = config.ip;
    port = config.port;
    index_path = config.index_path;
    root_path = config.root_path;
    log_path = config.log_path;
    disk_size = config.disk_size;
    namenode_addr = config.namenode_addr;
    namenode_port = config.namenode_port;
    paths = config.paths;
    init_paths = config.init_paths;
}
}
