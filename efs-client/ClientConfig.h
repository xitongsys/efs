#pragma once

#include <string>
#include <stdint.h>
#include <vector>

#include "HostDesc.h"
#include "NameNodeConn/NameNodeConn.h"
#include "DataNodeConn/DataNodeConn.h"

namespace efs {
	class ClientConfig
	{
	public:
		std::string user, password;
		std::string namenode_addr;
		uint16_t namenode_port;

	public:
		ClientConfig();
		ClientConfig(const std::string& config_file);
		ClientConfig(const ClientConfig& config);
	};
}

