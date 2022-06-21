#pragma once

#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "Conn.h"
#include "Error.h"
#include "FileDesc.h"

namespace efs {
	class DataNodeConn : public Conn {
	public:
		std::string ip;
		uint16_t port;
		std::string user, password;
	public:
		DataNodeConn(
			boost::asio::io_context& io_context,
			const std::string& ip, uint16_t port,
			const std::string& user, const std::string& password);

		ErrorCode login();
		ErrorCode getFileDesc(const std::string& path, FileDesc& fdesc);
		ErrorCode mkdir(const std::string& path);
		ErrorCode rm(const std::string& path);
		ErrorCode open(const std::string& path, const std::string& open_mod, int32_t& fd);
		ErrorCode write(const int32_t& fd, const std::string& data, int32_t& write_size);
		ErrorCode read(const int32_t& fd, const int32_t& read_size, std::string& data);
		ErrorCode close(const int32_t& fd);
		ErrorCode ls(const std::string& path, std::vector<FileDesc>& fdescs);

		ErrorCode openOffset(const std::string& path);
		ErrorCode readOffset(const std::string& path, const int32_t& read_size, const int64_t& offset, std::string& data);
		ErrorCode writeOffset(const std::string& path, const std::string& data, const int64_t& offset, int32_t& write_size);
		ErrorCode truncate(const std::string& path, const int64_t offset);
	};
}