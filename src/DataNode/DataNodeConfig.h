#pragma once

#include <string>

namespace efs {
class DataNodeConfig {
public:
    std::string name;
    std::string ip;
    uint16_t port;
    std::string log_path;
    std::string root_path;
    uint64_t disk_size;
    int32_t buffer_size;
    int32_t max_msg_size;

    std::string name_node_ip;
    uint16_t name_node_port;

    DataNodeConfig();
    DataNodeConfig(const std::string& file);
    DataNodeConfig(const DataNodeConfig& config);
};
}
