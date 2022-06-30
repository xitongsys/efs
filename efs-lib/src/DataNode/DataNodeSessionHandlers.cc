#include <algorithm>
#include <cmath>
#include <iostream>
#include <filesystem>

#include "FS.h"
#include "Limit.h"
#include "DataNode/DataNodeSession.h"
#include "Msg/DataNode/MsgChmod.h"
#include "Msg/DataNode/MsgChown.h"
#include "Msg/DataNode/MsgClose.h"
#include "Msg/DataNode/MsgLogin.h"
#include "Msg/DataNode/MsgLs.h"
#include "Msg/DataNode/MsgMkdir.h"
#include "Msg/DataNode/MsgOpen.h"
#include "Msg/DataNode/MsgPerm.h"
#include "Msg/DataNode/MsgRead.h"
#include "Msg/DataNode/MsgRm.h"
#include "Msg/DataNode/MsgWrite.h"
#include "Msg/DataNode/MsgGetFileDesc.h"
#include "Msg/DataNode/MsgReadOffset.h"
#include "Msg/DataNode/MsgWriteOffset.h"
#include "Msg/DataNode/MsgOpenOffset.h"
#include "Msg/DataNode/MsgTruncate.h"
#include "Msg/DataNode/MsgMv.h"

namespace efs {
	void DataNodeSession::login()
	{
		std::shared_ptr<MsgLogin> p_in_msg = std::static_pointer_cast<MsgLogin>(this->p_in_msg);
		std::shared_ptr<MsgLoginResp> p_out_msg = std::static_pointer_cast<MsgLoginResp>(p_msgs[MsgType::LOGIN_RESP]);
		p_out_msg->error_code = p_executor->login(p_in_msg->user, p_in_msg->password, this->udesc);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::getFileDesc()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgGetFileDesc> p_in_msg = std::static_pointer_cast<MsgGetFileDesc>(this->p_in_msg);
		std::shared_ptr<MsgGetFileDescResp> p_out_msg = std::static_pointer_cast<MsgGetFileDescResp>(p_msgs[MsgType::GET_FILE_DESC_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			p_out_msg->error_code = p_executor->getFileDesc(p_in_msg->path, p_out_msg->fdesc);

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::ls()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgLs> p_in_msg = std::static_pointer_cast<MsgLs>(this->p_in_msg);
		std::shared_ptr<MsgLsResp> p_out_msg = std::static_pointer_cast<MsgLsResp>(p_msgs[MsgType::LS_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			Permission perm = Permission::EMPTY;

			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
				p_out_msg->error_code = ec;
				break;
			}

			if (!(perm & Permission::R)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			p_out_msg->files.clear();
			if ((ec = p_executor->ls(p_in_msg->path, p_out_msg->files))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::rm()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgRm> p_in_msg = std::static_pointer_cast<MsgRm>(this->p_in_msg);
		std::shared_ptr<MsgRmResp> p_out_msg = std::static_pointer_cast<MsgRmResp>(p_msgs[MsgType::RM_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc fdesc;
			if ((ec = p_executor->getFileDesc(p_in_msg->path, fdesc))) {
				p_out_msg->error_code = ec;
				break;
			}

			Permission perm = fdesc.perm(this->udesc.uid, this->udesc.gid);
			if (!(perm & Permission::W)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			if (fdesc.mod & FileType::F_IFDIR) {
				std::vector<FileDesc> files;
				if ((ec = p_executor->ls(p_in_msg->path, files))) {
					p_out_msg->error_code = ec;
					break;
				}

				if (files.size()) {
					p_out_msg->error_code = ErrorCode::E_FILE_RM;
					break;
				}
			}

			// close open fd before rm
			std::vector<int32_t> removed;
			for (auto it = open_files.begin(); it != open_files.end(); it++) {
				if (it->second.fdesc.path == p_in_msg->path) {
					fclose(it->second.fp);
					removed.push_back(it->first);
				}
			}
			for (int32_t fd : removed) {
				open_files.erase(fd);
			}
			/////////////////

			if ((ec = p_executor->rm(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::mv()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgMv> p_in_msg = std::static_pointer_cast<MsgMv>(this->p_in_msg);
		std::shared_ptr<MsgMvResp> p_out_msg = std::static_pointer_cast<MsgMvResp>(p_msgs[MsgType::MV_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {

			if ((ec = fs::checkPath(p_in_msg->from_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			if ((ec = fs::checkPath(p_in_msg->to_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc to_fdesc;
			if (!(ec = p_executor->getFileDesc(p_in_msg->to_path, to_fdesc))) {
				//to file exists
				p_out_msg->error_code = ErrorCode::E_FILE_EXIST;
				break;
			}

			Permission from_perm = Permission::EMPTY;
			Permission to_perm = Permission::EMPTY;

			if ((ec = p_executor->permission(p_in_msg->from_path, this->udesc.uid, this->udesc.gid, from_perm))) {
				p_out_msg->error_code = ec;
				break;
			}

			std::string to_parent_path = "";
			if ((ec = p_executor->parentPath(p_in_msg->to_path, to_parent_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			if ((ec = p_executor->permission(to_parent_path, this->udesc.uid, this->udesc.gid, to_perm))) {
				p_out_msg->error_code = ec;
				break;
			}

			if (!(from_perm & Permission::W) || !(to_perm & Permission::W)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;

			}

			std::string from_absolute_path = "";
			if ((ec = p_executor->absolutePath(p_in_msg->from_path, from_absolute_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			std::string to_absolute_path = "";
			if ((ec = p_executor->absolutePath(p_in_msg->to_path, to_absolute_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc from_fdesc;
			if ((ec = p_executor->getFileDesc(p_in_msg->from_path, from_fdesc))) {
				p_out_msg->error_code = ec;
				break;
			}

			// mv can only used for regular file
			if (!(from_fdesc.mod & FileType::F_IFREG)) {
				p_out_msg->error_code = ErrorCode::E_FILE_MV;
				break;
			}

			// close open fd before rename
			std::vector<int32_t> removed;
			for (auto it = open_files.begin(); it != open_files.end(); it++) {
				if (it->second.fdesc.path == p_in_msg->from_path) {
					fclose(it->second.fp);
					removed.push_back(it->first);
				}
			}
			for (int32_t fd : removed) {
				open_files.erase(fd);
			}
			/////////////////

			std::error_code std_ec;
			std::filesystem::rename(from_absolute_path, to_absolute_path, std_ec);
			if (std_ec) {
				p_out_msg->error_code = ErrorCode::E_FILE_MV;
				break;
			}

			to_fdesc = from_fdesc;
			to_fdesc.path = p_in_msg->to_path;
			if ((ec = p_executor->setFileDesc(p_in_msg->to_path, to_fdesc))) {
				p_out_msg->error_code = ec;
				break;
			}

			if ((ec = p_executor->rmFileDesc(p_in_msg->from_path))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::chown()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgChown> p_in_msg = std::static_pointer_cast<MsgChown>(this->p_in_msg);
		std::shared_ptr<MsgChownResp> p_out_msg = std::static_pointer_cast<MsgChownResp>(p_msgs[MsgType::CHOWN_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			Permission perm = Permission::EMPTY;
			if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
				p_out_msg->error_code = ec;
				break;
			}

			if (!(perm & Permission::W)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			UserDesc udesc;
			if ((ec = p_executor->getUser(p_in_msg->user, udesc))) {
				p_out_msg->error_code = ec;
				break;
			}

			if ((ec = p_executor->chown(p_in_msg->path, udesc.uid, udesc.gid))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::chmod()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgChmod> p_in_msg = std::static_pointer_cast<MsgChmod>(this->p_in_msg);
		std::shared_ptr<MsgChmodResp> p_out_msg = std::static_pointer_cast<MsgChmodResp>(p_msgs[MsgType::CHMOD_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			Permission perm = Permission::EMPTY;

			if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
				p_out_msg->error_code = ec;
				break;
			}
			if (!(perm & Permission::W)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			if ((ec = p_executor->chmod(p_in_msg->path, p_in_msg->mod))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::perm()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgPerm> p_in_msg = std::static_pointer_cast<MsgPerm>(this->p_in_msg);
		std::shared_ptr<MsgPermResp> p_out_msg = std::static_pointer_cast<MsgPermResp>(p_msgs[MsgType::PERM_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			Permission perm = Permission::EMPTY;
			FileDesc fdesc;
			UserDesc udesc;
			GroupDesc gdesc;
			int16_t id = 0;

			if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
				p_out_msg->error_code = ec;
				break;
			}

			if ((perm & Permission::W) == 0) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			if (p_in_msg->perm_type == PermType::USER) {
				if ((ec = p_executor->getUser(p_in_msg->name, udesc))) {
					p_out_msg->error_code = ec;
					break;
				}
				id = udesc.uid;

			}
			else if (p_in_msg->perm_type == PermType::GROUP) {
				if ((ec = p_executor->getGroup(p_in_msg->name, gdesc))) {
					p_out_msg->error_code = ec;
					break;
				}
				id = gdesc.gid;

			}
			else { // PermType::OTHER
			}

			if ((ec = p_executor->perm(p_in_msg->path, id, p_in_msg->perm_type, p_in_msg->perm))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::mkdir()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgMkdir> p_in_msg = std::static_pointer_cast<MsgMkdir>(this->p_in_msg);
		std::shared_ptr<MsgMkdirResp> p_out_msg = std::static_pointer_cast<MsgMkdirResp>(p_msgs[MsgType::MKDIR_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			std::string parent_path;
			if ((ec = p_executor->parentPath(p_in_msg->path, parent_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc parent_desc;
			if ((ec = p_executor->getFileDesc(parent_path, parent_desc))) {
				p_out_msg->error_code = ec;
				break;
			}

			Permission perm = parent_desc.perm(this->udesc.uid, this->udesc.gid);
			if ((perm & Permission::W)) {
				ec = p_executor->mkdir(p_in_msg->path, this->udesc.uid, this->udesc.gid, parent_desc);
				p_out_msg->error_code = ec;

			}
			else {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::open()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgOpen> p_in_msg = std::static_pointer_cast<MsgOpen>(this->p_in_msg);
		std::shared_ptr<MsgOpenResp> p_out_msg = std::static_pointer_cast<MsgOpenResp>(p_msgs[MsgType::OPEN_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		const std::string& path = p_in_msg->path;
		std::uint32_t flag = p_in_msg->flag;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			if (this->open_files.size() >= DataNodeSession::MAX_OPEN_FILE_NUM) {
				p_out_msg->error_code = E_FILE_TOO_MANY;
				break;
			}

			FileDesc fdesc;
			if ((ec = p_executor->getFileDesc(path, fdesc))) {
				std::string parent_path;
				if ((ec = p_executor->parentPath(path, parent_path))) {
					p_out_msg->error_code = ec;
					break;
				}

				FileDesc parent_fdesc;
				if ((ec = p_executor->getFileDesc(parent_path, parent_fdesc))) {
					p_out_msg->error_code = ec;
					break;
				}

				Permission perm = parent_fdesc.perm(this->udesc.uid, this->udesc.gid);

				if (!(perm & Permission::W)) {
					p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
					break;
				}

				if((flag & OpenFlag::OF_ACCMODE) == 0) {
					p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
					break;
				}

				OpenFileHandler fh;
				if ((ec = p_executor->open(path, "wb+", this->udesc.uid, this->udesc.gid, parent_fdesc, fh))) {
					p_out_msg->error_code = ec;
					break;
				}

				this->open_files[fh.fd] = fh;
				p_out_msg->fd = fh.fd;

			}
			else {
				OpenFileHandler fh;
				std::string mode = openFlagToMode(OpenFlag(flag));
				if ((ec = p_executor->open(path, mode, this->udesc.uid, this->udesc.gid, fdesc, fh))) {
					p_out_msg->error_code = ec;
					break;
				}

				this->open_files[fh.fd] = fh;
				p_out_msg->fd = fh.fd;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::close()
	{
		std::shared_ptr<MsgClose> p_in_msg = std::static_pointer_cast<MsgClose>(this->p_in_msg);
		std::shared_ptr<MsgCloseResp> p_out_msg = std::static_pointer_cast<MsgCloseResp>(p_msgs[MsgType::CLOSE_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		int32_t fd = p_in_msg->fd;
		if (this->open_files.count(fd)) {
			p_executor->close(this->open_files[fd]);

			OpenFileHandler& fh = this->open_files[fd];
			FileDesc fdesc;
			if (!(p_executor->getFileDescFromDisk(fh.fdesc.path, fdesc))) {
				// actually only modified_time and fsize can be get from the host filesystem

				fh.fdesc.modified_time = fdesc.modified_time;
				fh.fdesc.fsize = fdesc.fsize;

				// TODO:: if error, what should do ?
				p_executor->setFileDesc(fh.fdesc.path, fh.fdesc);
			}

			this->open_files.erase(fd);
		}

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::read()
	{
		std::shared_ptr<MsgRead> p_in_msg = std::static_pointer_cast<MsgRead>(this->p_in_msg);
		std::shared_ptr<MsgReadResp> p_out_msg = std::static_pointer_cast<MsgReadResp>(p_msgs[MsgType::READ_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if (this->open_files.count(p_in_msg->fd) == 0) {
				p_out_msg->error_code = E_FILE_READ;
				break;
			}

			OpenFileHandler& fh = this->open_files[p_in_msg->fd];
			int32_t max_read_size = std::min(p_in_msg->read_size, EFS_MAX_READ_SIZE);
			p_out_msg->data.resize(max_read_size, 0);

			int64_t pos = ftell(fh.fp);
			if (pos < 0) {
				p_out_msg->error_code = E_FILE_READ;
				break;
			}

			if (pos != p_in_msg->offset) {
				pos = p_in_msg->offset;
				if(fseek(fh.fp, pos, SEEK_SET)){
					p_out_msg->error_code = E_FILE_READ;
					break;
				}
			}

			int32_t read_size = fread(p_out_msg->data.data(), 1, max_read_size, fh.fp);
			if (read_size < max_read_size) {
				p_out_msg->data.resize(read_size);
				if (feof(fh.fp)) {
					p_out_msg->error_code = ErrorCode::E_FILE_EOF;

				}
				else {
					p_out_msg->error_code = ErrorCode::E_FILE_READ;
				}
			}

		} while (0);
		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::write()
	{
		std::shared_ptr<MsgWrite> p_in_msg = std::static_pointer_cast<MsgWrite>(this->p_in_msg);
		std::shared_ptr<MsgWriteResp> p_out_msg = std::static_pointer_cast<MsgWriteResp>(p_msgs[MsgType::WRITE_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if (this->open_files.count(p_in_msg->fd) == 0) {
				p_out_msg->error_code = E_FILE_WRITE;
				p_out_msg->write_size = 0;
				break;
			}

			OpenFileHandler& fh = this->open_files[p_in_msg->fd];

			int64_t pos = ftell(fh.fp);
			if (pos < 0) {
				p_out_msg->error_code = E_FILE_WRITE;
				p_out_msg->write_size = 0;
				break;
			}

			if (pos != p_in_msg->offset) {
				pos = p_in_msg->offset;
				if(fseek(fh.fp, pos, SEEK_SET)) {
					p_out_msg->error_code = E_FILE_WRITE;
					p_out_msg->write_size = 0;
					break;
				}
			}

			int32_t write_size = fwrite(p_in_msg->data.c_str(), 1, p_in_msg->data.size(), fh.fp);
			p_out_msg->write_size = write_size;
			if (write_size != int32_t(p_in_msg->data.size())) {
				p_out_msg->error_code = E_FILE_WRITE;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::openOffset()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgOpenOffset> p_in_msg = std::static_pointer_cast<MsgOpenOffset>(this->p_in_msg);
		std::shared_ptr<MsgOpenOffsetResp> p_out_msg = std::static_pointer_cast<MsgOpenOffsetResp>(p_msgs[MsgType::OPENOFFSET_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		const std::string& path = p_in_msg->path;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc fdesc;
			if ((ec = p_executor->getFileDesc(path, fdesc))) {
				std::string parent_path;
				if ((ec = p_executor->parentPath(path, parent_path))) {
					p_out_msg->error_code = ec;
					break;
				}

				FileDesc parent_fdesc;
				if ((ec = p_executor->getFileDesc(parent_path, parent_fdesc))) {
					p_out_msg->error_code = ec;
					break;
				}

				Permission perm = parent_fdesc.perm(this->udesc.uid, this->udesc.gid);

				if (!(perm & Permission::W)) {
					p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
					break;
				}

				OpenFileHandler fh;
				if ((ec = p_executor->open(path, "wb+", this->udesc.uid, this->udesc.gid, parent_fdesc, fh))) {
					p_out_msg->error_code = ec;
					break;
				}

				p_executor->close(fh);
			}
		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::readOffset()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgReadOffset> p_in_msg = std::static_pointer_cast<MsgReadOffset>(this->p_in_msg);
		std::shared_ptr<MsgReadOffsetResp> p_out_msg = std::static_pointer_cast<MsgReadOffsetResp>(p_msgs[MsgType::READOFFSET_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc fdesc;
			if ((ec = p_executor->getFileDesc(p_in_msg->path, fdesc))) {
				p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
				break;
			}

			Permission perm = fdesc.perm(this->udesc.uid, this->udesc.gid);

			if (!(perm & Permission::R)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			if (p_in_msg->read_size > EFS_MAX_READ_SIZE) {
				p_out_msg->error_code = ErrorCode::E_OVERFLOW;
				break;
			}

			std::string absolute_path;
			if ((ec = p_executor->absolutePath(p_in_msg->path, absolute_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FILE* fp = fopen(absolute_path.c_str(), "rb");
			if (fp == nullptr) {
				p_out_msg->error_code = ErrorCode::E_FILE_OPEN;
				break;
			}

			if (p_in_msg->offset > 0) {
				int64_t pos = p_in_msg->offset;
				if(fseek(fp, pos, SEEK_SET)) {
					p_out_msg->error_code = ErrorCode::E_FILE_SEEK;
					break;
				}
			}

			p_out_msg->data.resize(p_in_msg->read_size, 0);
			int32_t read_size = fread(p_out_msg->data.data(), 1, p_in_msg->read_size, fp);

			p_out_msg->data.resize(read_size);

			if (read_size != p_in_msg->read_size) {
				if (feof(fp)) {
					p_out_msg->error_code = ErrorCode::E_FILE_EOF;
				}
				else if (ferror(fp)) {
					p_out_msg->error_code = ErrorCode::E_FILE_READ;
				}
			}

			fclose(fp);

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::writeOffset()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgWriteOffset> p_in_msg = std::static_pointer_cast<MsgWriteOffset>(this->p_in_msg);
		std::shared_ptr<MsgWriteOffsetResp> p_out_msg = std::static_pointer_cast<MsgWriteOffsetResp>(p_msgs[MsgType::WRITEOFFSET_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc fdesc;
			if ((ec = p_executor->getFileDesc(p_in_msg->path, fdesc))) {
				p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
				break;
			}

			Permission perm = fdesc.perm(this->udesc.uid, this->udesc.gid);

			if (!(perm & Permission::W)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			std::string absolute_path;
			if ((ec = p_executor->absolutePath(p_in_msg->path, absolute_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FILE* fp = fopen(absolute_path.c_str(), "rb+");
			if (fp == nullptr) {
				p_out_msg->error_code = ErrorCode::E_FILE_OPEN;
				break;
			}
			int64_t pos = p_in_msg->offset;
			if (p_in_msg->offset > 0) {
				if(fseek(fp, pos, SEEK_SET)) {
					p_out_msg->error_code = ErrorCode::E_FILE_SEEK;
					break;
				}
			}

			int32_t write_size = fwrite(p_in_msg->data.c_str(), 1, p_in_msg->data.size(), fp);

			p_out_msg->write_size = write_size;

			if (write_size != int(p_in_msg->data.size())) {
				p_out_msg->error_code = ErrorCode::E_FILE_WRITE;
			}

			fclose(fp);

			fdesc.fsize = fs::fileSize(absolute_path);
			fdesc.modified_time = util::now();

			if ((ec = p_executor->setFileDesc(p_in_msg->path, fdesc))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}
	void DataNodeSession::truncate()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgTruncate> p_in_msg = std::static_pointer_cast<MsgTruncate>(this->p_in_msg);
		std::shared_ptr<MsgTruncateResp> p_out_msg = std::static_pointer_cast<MsgTruncateResp>(p_msgs[MsgType::TRUNCATE_RESP]);
		p_out_msg->error_code = ErrorCode::NONE;

		do {
			if ((ec = fs::checkPath(p_in_msg->path))) {
				p_out_msg->error_code = ec;
				break;
			}

			FileDesc fdesc;
			if ((ec = p_executor->getFileDesc(p_in_msg->path, fdesc))) {
				p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
				break;
			}

			Permission perm = fdesc.perm(this->udesc.uid, this->udesc.gid);

			if (!(perm & Permission::W)) {
				p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
				break;
			}

			std::string absolute_path;
			if ((ec = p_executor->absolutePath(p_in_msg->path, absolute_path))) {
				p_out_msg->error_code = ec;
				break;
			}

			std::error_code s_ec;
			std::filesystem::resize_file(absolute_path, p_in_msg->offset, s_ec);
			if (s_ec) {
				p_out_msg->error_code = ErrorCode::E_FILE_TRUNCATE;
				break;
			}

			fdesc.fsize = p_in_msg->offset;
			if ((ec = p_executor->setFileDesc(p_in_msg->path, fdesc))) {
				p_out_msg->error_code = ec;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}
}
