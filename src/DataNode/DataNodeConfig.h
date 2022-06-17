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

    std::string log_path;
    std::string root_path;

    uint64_t disk_size;

    int32_t buffer_size;
    int32_t max_msg_size;

    std::string name_node_ip;
    uint16_t name_node_port;

    //["user01,pwd01,1,1,/user01", "user02,pwd02,2,1,/user02"]
    std::vector<std::string> users;

    DataNodeConfig();
    DataNodeConfig(const std::string& file);
    DataNodeConfig(const DataNodeConfig& config);
};
}
