#include <yaml-cpp/yaml.h>

#include "ClientConfig.h"

namespace efs {
	ClientConfig::ClientConfig()
	{
		user = "";
		password = "";
		namenode_addr = "";
		namenode_port = 0;
	}

	ClientConfig::ClientConfig(const std::string& config_file)
	{
		YAML::Node node = YAML::LoadFile(config_file);
		user = node["user"].as<std::string>();
		password = node["password"].as<std::string>();
		namenode_addr = node["namenode_addr"].as<std::string>();
		namenode_port = node["namenode_port"].as<uint16_t>();
	}

	ClientConfig::ClientConfig(const ClientConfig& config)
	{
		user = config.user;
		password = config.password;
		namenode_addr = config.namenode_addr;
		namenode_port = config.namenode_port;
	}
}