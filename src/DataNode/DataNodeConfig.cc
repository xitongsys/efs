#include <yaml-cpp/yaml.h>

#include "DataNodeConfig.h"

namespace efs {
DataNodeConfig::DataNodeConfig() { }

DataNodeConfig::DataNodeConfig(const std::string& file_)
{
    YAML::Node node = YAML::LoadFile(file_);

    name = node["name"].as<std::string>();
    ip = node["ip"].as<std::string>();
    port = node["port"].as<uint16_t>();
    log_path = node["log_path"].as<std::string>();
    disk_size = node["disk_size"].as<uint64_t>();

    name_node_ip = node["name_node_ip"].as<std::string>();
    name_node_port = node["name_node_port"].as<uint16_t>();
}

DataNodeConfig::DataNodeConfig(const DataNodeConfig& config_)
{
    name = config_.name;
    ip = config_.ip;
    port = config_.port;
    log_path = config_.log_path;
    disk_size = config_.disk_size;
    name_node_ip = config_.name_node_ip;
    name_node_port = config_.name_node_port;
}
}
