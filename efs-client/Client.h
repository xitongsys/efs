#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>

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

		// <path, host idx, p_conn>
		std::vector<std::tuple<std::string, int, std::shared_ptr<DataNodeConn>>> p_conns;

		char* buffer;

	public:
		Client(const ClientConfig& config);
		~Client();

		ErrorCode getDataNodes();
		ErrorCode getDataNodeConn(const std::string& path, std::shared_ptr<DataNodeConn>& p_datanode);
		ErrorCode getAllDataNodeConns(std::vector<std::shared_ptr<DataNodeConn>>& p_conns);

	public:
		ErrorCode getFileDesc(const std::string& path, FileDesc& fdesc);
		ErrorCode mkdir(const std::string& path);
		ErrorCode rm(const std::string& path);
		ErrorCode mv(const std::string& from_path, const std::string& to_path);
		ErrorCode ls(const std::string& path, std::vector<FileDesc>& fdescs);
		ErrorCode cp(const std::string& from_path, const std::string& to_path, std::shared_ptr<DataNodeConn> p_from_conn, std::shared_ptr<DataNodeConn> p_to_conn);

		ErrorCode openOffset(const std::string& path);

		ErrorCode readOffset(const std::string& path, const int64_t& read_size, const int64_t& offset, char* data, int64_t& real_read_size);
		ErrorCode writeOffset(const std::string& path, const char* data, int64_t write_size, const int64_t& offset, int64_t& real_write_size);

		ErrorCode truncate(const std::string& path, const int64_t offset);
	};
}

