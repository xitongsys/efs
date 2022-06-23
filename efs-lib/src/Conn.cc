#include "Conn.h"
#include "Limit.h"

namespace efs {
	Conn::Conn(boost::asio::io_context& io_context, const std::string& ip, uint16_t port) :
		io_context(io_context),
		sock(io_context),
		resolver(io_context),
		ip(ip),
		port(port)
	{
		boost::asio::connect(sock, resolver.resolve(ip, std::to_string(port)));
		buf = new char[EFS_BUFFER_SIZE];
	}

	Conn::~Conn()
	{
		boost::system::error_code b_ec;
		sock.close(b_ec);
		delete[] buf;
	}

	ErrorCode Conn::closeConn()
	{
		boost::system::error_code b_ec;
		sock.close(b_ec);
		return ErrorCode::NONE;
	}
}
