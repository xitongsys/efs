#pragma once

#include <string>

namespace efs {
    class DataNodeConfig {
        public:
        std::string name;
        std::string ip;
        uint16_t port;
        std::string log_path;
        uint64_t disk_size;


        std::string name_node_ip;
        uint16_t name_node_port;
        
        DataNodeConfig(const std::string& file_);
        DataNodeConfig(const DataNodeConfig& config_);
    };
}
