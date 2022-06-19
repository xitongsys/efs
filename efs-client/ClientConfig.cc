#include <yaml-cpp/yaml.h>

#include "ClientConfig.h"

namespace efs {
	ClientConfig::ClientConfig()
	{
		user = "";
		password = "";
		namenode_ip = "";
		namenode_port = 0;
	}

	ClientConfig::ClientConfig(const std::string& config_file)
	{
		YAML::Node node = YAML::LoadFile(config_file);
		user = node["user"].as<std::string>();
		password = node["password"].as<std::string>();
		namenode_ip = node["namenode_ip"].as<std::string>();
		namenode_port = node["namenode_port"].as<uint16_t>();
	}
}