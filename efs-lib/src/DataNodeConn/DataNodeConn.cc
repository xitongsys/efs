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
#include "Msg/DataNode/MsgOpenOffset.h"
#include "Msg/DataNode/MsgReadOffset.h"
#include "Msg/DataNode/MsgWriteOffset.h"
#include "Msg/DataNode/MsgTruncate.h"
#include "Msg/DataNode/MsgMv.h"
#include "Msg/DataNode/MsgPerm.h"
#include "Msg/DataNode/MsgChown.h"

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

	ErrorCode DataNodeConn::mv(const std::string& from_path, const std::string& to_path)
	{
		MsgMv m_mv;
		m_mv.from_path = from_path;
		m_mv.to_path = to_path;

		MsgMvResp m_mv_resp;
		query<MsgMv, MsgMvResp>(m_mv, m_mv_resp);

		return ErrorCode(m_mv_resp.error_code);
	}

	ErrorCode DataNodeConn::open(const std::string& path, const int32_t& flag, int32_t& fd)
	{
		MsgOpen m_open;
		m_open.path = path;
		m_open.flag = flag;

		MsgOpenResp m_open_resp;
		query<MsgOpen, MsgOpenResp>(m_open, m_open_resp);
		fd = m_open_resp.fd;

		return ErrorCode(m_open_resp.error_code);
	}

	ErrorCode DataNodeConn::write(const int32_t& fd, const std::string& data, const int64_t& offset, int32_t& write_size)
	{
		MsgWrite m_write;
		m_write.data = std::move(data);
		m_write.fd = fd;
		m_write.offset = offset;

		MsgWriteResp m_write_resp;
		query<MsgWrite, MsgWriteResp>(m_write, m_write_resp);
		write_size = m_write_resp.write_size;

		return ErrorCode(m_write_resp.error_code);
	}

	ErrorCode DataNodeConn::read(const int32_t& fd, const int32_t& read_size, const int64_t& offset, std::string& data)
	{
		MsgRead m_read;
		m_read.fd = fd;
		m_read.read_size = read_size;
		m_read.offset = offset;

		MsgReadResp m_read_resp;
		query<MsgRead, MsgReadResp>(m_read, m_read_resp);

		data = std::move(m_read_resp.data);
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

	ErrorCode DataNodeConn::perm(const std::string& path, const std::string& name, const PermType& perm_type, const Permission& perm)
	{
		MsgPerm m_perm;
		m_perm.path = path;
		m_perm.name = name;
		m_perm.perm_type = perm_type;
		m_perm.perm = perm;

		MsgPermResp m_perm_resp;
		query<MsgPerm, MsgPermResp>(m_perm, m_perm_resp);
		return ErrorCode(m_perm_resp.error_code);
	}

	ErrorCode DataNodeConn::chown(const std::string& path, const std::string& user)
	{
		MsgChown m_chown;
		m_chown.path = path;
		m_chown.user = user;

		MsgChownResp m_chown_resp;
		query<MsgChown, MsgChownResp>(m_chown, m_chown_resp);
		return ErrorCode(m_chown_resp.error_code);
	}

	ErrorCode DataNodeConn::openOffset(const std::string& path)
	{
		MsgOpenOffset m_openoffset;
		m_openoffset.path = path;

		MsgOpenOffsetResp m_openoffset_resp;
		query<MsgOpenOffset, MsgOpenOffsetResp>(m_openoffset, m_openoffset_resp);
		return ErrorCode(m_openoffset_resp.error_code);
	}

	ErrorCode DataNodeConn::readOffset(const std::string& path, const int32_t& read_size, const int64_t& offset, std::string& data)
	{
		MsgReadOffset m_readoffset;
		m_readoffset.path = path;
		m_readoffset.read_size = read_size;
		m_readoffset.offset = offset;

		MsgReadOffsetResp m_readoffset_resp;
		query<MsgReadOffset, MsgReadOffsetResp>(m_readoffset, m_readoffset_resp);
		data = m_readoffset_resp.data;
		return ErrorCode(m_readoffset_resp.error_code);
	}

	ErrorCode DataNodeConn::writeOffset(const std::string& path, const std::string& data, const int64_t& offset, int32_t& write_size)
	{
		MsgWriteOffset m_writeoffset;
		m_writeoffset.path = path;
		m_writeoffset.data = data;
		m_writeoffset.offset = offset;

		MsgWriteOffsetResp m_writeoffset_resp;
		query<MsgWriteOffset, MsgWriteOffsetResp>(m_writeoffset, m_writeoffset_resp);
		write_size = m_writeoffset_resp.write_size;
		return ErrorCode(m_writeoffset_resp.error_code);
	}

	ErrorCode DataNodeConn::readOffset(const std::string& path, const int32_t& read_size, const int64_t& offset, char* data, int32_t& real_read_size)
	{
		MsgReadOffset m_readoffset;
		m_readoffset.path = path;
		m_readoffset.read_size = read_size;
		m_readoffset.offset = offset;

		MsgReadOffsetResp m_readoffset_resp;
		query<MsgReadOffset, MsgReadOffsetResp>(m_readoffset, m_readoffset_resp);
		real_read_size = m_readoffset_resp.data.size();
		memcpy(data, m_readoffset_resp.data.c_str(), real_read_size);
		return ErrorCode(m_readoffset_resp.error_code);
	}

	ErrorCode DataNodeConn::writeOffset(const std::string& path, const char* data, int32_t write_size, const int64_t& offset, int32_t& real_write_size)
	{
		MsgWriteOffset m_writeoffset;
		m_writeoffset.path = path;
		m_writeoffset.data = std::string(data, write_size);
		m_writeoffset.offset = offset;

		MsgWriteOffsetResp m_writeoffset_resp;
		query<MsgWriteOffset, MsgWriteOffsetResp>(m_writeoffset, m_writeoffset_resp);
		real_write_size = m_writeoffset_resp.write_size;
		return ErrorCode(m_writeoffset_resp.error_code);
	}

	ErrorCode DataNodeConn::truncate(const std::string& path, const int64_t offset)
	{
		MsgTruncate m_truncate;
		m_truncate.path = path;
		m_truncate.offset = offset;

		MsgTruncateResp m_truncate_resp;
		query<MsgTruncate, MsgTruncateResp>(m_truncate, m_truncate_resp);
		return ErrorCode(m_truncate_resp.error_code);
	}
}