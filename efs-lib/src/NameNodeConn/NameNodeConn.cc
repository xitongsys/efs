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

	ErrorCode NameNodeConn::hosts(const std::string& user, const std::string& password, std::vector<HostDesc>& hosts, UserDesc& udesc)
	{
		MsgHost m_host;
		MsgHostResp m_host_resp;
		m_host.user = user;
		m_host.password = password;
		query<MsgHost, MsgHostResp>(m_host, m_host_resp);
		hosts = m_host_resp.hosts;
		udesc = m_host_resp.udesc;
		return ErrorCode(m_host_resp.error_code);
	}
}