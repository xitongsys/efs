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
	};
}