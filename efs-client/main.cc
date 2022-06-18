#include <iostream>
#include <boost/asio.hpp>

#include "NameNodeConn/NameNodeConn.h"
#include "Msg/NameNode/MsgHost.h"

int main()
{
	boost::asio::io_context io_context;
	efs::NameNodeConn conn(io_context, "127.0.0.1", 10000, "");

	efs::MsgHostResp m_host_resp;
	conn.host(m_host_resp);

	std::cout << m_host_resp.hosts.size() << std::endl;
	return 0;
}