#include "Conn.h"

namespace efs {
	Conn::Conn(boost::asio::io_context& io_context, const std::string& ip, uint16_t port) :
		io_context(io_context),
		sock(io_context),
		resolver(io_context),
		ip(ip),
		port(port)
	{
		boost::asio::connect(sock, resolver.resolve(ip, std::to_string(port)));
	}

	ErrorCode Conn::closeConn()
	{
		sock.close();
		return ErrorCode::NONE;
	}
}
