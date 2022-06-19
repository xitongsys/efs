#include "DataNodeConn.h"
#include "Msg/DataNode/MsgLogin.h"

namespace efs {
	DataNodeConn::DataNodeConn(
		boost::asio::io_context& io_context,
		const std::string& ip, uint16_t port,
		const std::string& user, const std::string& password) :
		Conn(io_context, ip, port),
		ip(ip), port(port), user(user), password(password) 
	{
	}

	ErrorCode DataNodeConn::login()
	{
		MsgLogin m_login;
		m_login.user = user;
		m_login.password = password;

		MsgLoginResp m_login_resp;
		query<MsgLogin, MsgLoginResp>(m_login, m_login_resp);
		return ErrorCode(m_login_resp.error_code);
	}
}