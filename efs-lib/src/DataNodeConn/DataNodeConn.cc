#include "DataNodeConn.h"
#include "Msg/DataNode/MsgLogin.h"
#include "Msg/DataNode/MsgGetFileDesc.h"
#include "Msg/DataNode/MsgMkdir.h"
#include "Msg/DataNode/MsgRm.h"
#include "Msg/DataNode/MsgOpen.h"
#include "Msg/DataNode/MsgRead.h"
#include "Msg/DataNode/MsgWrite.h"
#include "Msg/DataNode/MsgClose.h"
#include "Msg/DataNode/MsgLs.h"

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

	ErrorCode DataNodeConn::open(const std::string& path, const std::string& open_mod, int32_t& fd)
	{
		MsgOpen m_open;
		m_open.path = path;
		m_open.open_mod = open_mod;

		MsgOpenResp m_open_resp;
		query<MsgOpen, MsgOpenResp>(m_open, m_open_resp);
		fd = m_open_resp.fd;

		return ErrorCode(m_open_resp.error_code);
	}

	ErrorCode DataNodeConn::write(const int32_t& fd, const std::string& data, int32_t& write_size)
	{
		MsgWrite m_write;
		m_write.data = data;
		m_write.fd = fd;

		MsgWriteResp m_write_resp;
		query<MsgWrite, MsgWriteResp>(m_write, m_write_resp);
		write_size = m_write_resp.write_size;

		return ErrorCode(m_write_resp.error_code);
	}

	ErrorCode DataNodeConn::read(const int32_t& fd, const int32_t& read_size, std::string& data)
	{
		MsgRead m_read;
		m_read.fd = fd;
		m_read.read_size = read_size;

		MsgReadResp m_read_resp;
		query<MsgRead, MsgReadResp>(m_read, m_read_resp);

		data = m_read_resp.data;
		return ErrorCode(m_read_resp.error_code);
	}

	ErrorCode DataNodeConn::close(const int32_t& fd)
	{
		MsgClose m_close;
		m_close.fd = fd;

		MsgCloseResp m_close_resp;
		query<MsgClose, MsgCloseResp>(m_close, m_close_resp);
		return ErrorCode(m_close_resp.error_code);
	}

	ErrorCode DataNodeConn::ls(const std::string& path, std::vector<FileDesc>& fdescs)
	{
		MsgLs m_ls;
		m_ls.path = path;

		MsgLsResp m_ls_resp;
		query<MsgLs, MsgLsResp>(m_ls, m_ls_resp);
		fdescs = m_ls_resp.files;
		return ErrorCode(m_ls_resp.error_code);
	}
}