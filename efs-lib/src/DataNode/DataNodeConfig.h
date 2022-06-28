#pragma once

#include <string>
#include <vector>

namespace efs {
class DataNodeConfig {
public:
    std::string name;
    std::string token;

    std::string ip;
    uint16_t port;

    std::string index_path;
    std::string log_path;
    std::string root_path;

    uint64_t disk_size;

    std::string namenode_addr;
    uint16_t namenode_port;

    std::vector<std::string> paths;

    std::vector<std::string> init_paths;

    DataNodeConfig();
    DataNodeConfig(const std::string& file);
    DataNodeConfig(const DataNodeConfig& config);
};
}
