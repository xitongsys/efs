#include "NameNodeConn.h"

namespace efs {

	NameNodeConn::NameNodeConn(boost::asio::io_context& io_context, 
		const std::string& ip, const uint16_t& port,
		const std::string& token) :
		Conn(io_context, ip, port),
		ip(ip), port(port), 
		token(token) 
	{
	}

	ErrorCode NameNodeConn::host(MsgHostResp& m_host_resp) 
	{
		MsgHost m_host;
		return query<MsgHost, MsgHostResp>(m_host, m_host_resp);
	}
}