#include "DataNodeConn.h"
#include "Msg/DataNode/MsgLogin.h"
#include "Msg/DataNode/MsgGetFileDesc.h"
#include "Msg/DataNode/MsgMkdir.h"
#include "Msg/DataNode/MsgRm.h"

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

	ErrorCode DataNodeConn::getFileDesc(const std::string& path, FileDesc& fdesc)
	{
		MsgGetFileDesc m_get_file_desc;
		m_get_file_desc.path = path;

		MsgGetFileDescResp m_get_file_desc_resp;
		query<MsgGetFileDesc, MsgGetFileDescResp>(m_get_file_desc, m_get_file_desc_resp);
		fdesc = m_get_file_desc_resp.fdesc;

		return ErrorCode(m_get_file_desc_resp.error_code);
	}

	ErrorCode DataNodeConn::mkdir(const std::string& path)
	{
		MsgMkdir m_mkdir;
		m_mkdir.path = path;

		MsgMkdirResp m_mkdir_resp;
		query<MsgMkdir, MsgMkdirResp>(m_mkdir, m_mkdir_resp);

		return ErrorCode(m_mkdir_resp.error_code);
	}

	ErrorCode DataNodeConn::rm(const std::string& path)
	{
		MsgRm m_rm;
		m_rm.path = path;

		MsgRmResp m_rm_resp;
		query<MsgRm, MsgRmResp>(m_rm, m_rm_resp);

		return ErrorCode(m_rm_resp.error_code);
	}
}