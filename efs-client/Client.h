#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include <boost/asio.hpp>

#include "ClientConfig.h"
#include "NameNodeConn/NameNodeConn.h"
#include "DataNodeConn/DataNodeConn.h"

namespace efs {
	class Client
	{
	public:
		boost::asio::io_context io_context;
		ClientConfig config;

		UserDesc udesc;
		std::vector<HostDesc> hosts;

		std::unordered_map<std::string, std::shared_ptr<DataNodeConn>> p_conns;

	public:
		Client(const ClientConfig& config);

		ErrorCode getDataNodes();
		ErrorCode getDataNodeConn(const std::string& path, std::shared_ptr<DataNodeConn>& p_datanode);
		ErrorCode getAllDataNodeConns(std::vector<std::shared_ptr<DataNodeConn>>& p_conns);
	};
}

