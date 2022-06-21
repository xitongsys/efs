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

    root_path = fs::formatPath(node["root_path"].as<std::string>());
    log_path = fs::formatPath(node["log_path"].as<std::string>());
    disk_size = node["disk_size"].as<uint64_t>();

    name_node_ip = node["name_node_ip"].as<std::string>();
    name_node_port = node["name_node_port"].as<uint16_t>();

    paths = node["paths"].as<std::vector<std::string>>();
    init_paths = node["init_paths"].as<std::vector<std::string>>();
}

DataNodeConfig::DataNodeConfig(const DataNodeConfig& config)
{
    name = config.name;
    token = config.token;
    ip = config.ip;
    port = config.port;
    root_path = config.root_path;
    log_path = config.log_path;
    disk_size = config.disk_size;
    name_node_ip = config.name_node_ip;
    name_node_port = config.name_node_port;
    paths = config.paths;
    init_paths = config.init_paths;
}
}
