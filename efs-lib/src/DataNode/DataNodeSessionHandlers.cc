#include <algorithm>
#include <cmath>
#include <iostream>

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

namespace efs {
	void DataNodeSession::login()
	{
		std::shared_ptr<MsgLogin> p_in_msg = std::static_pointer_cast<MsgLogin>(this->p_in_msg);
		std::shared_ptr<MsgLoginResp> p_out_msg = std::make_shared<MsgLoginResp>();
		p_out_msg->error_code = p_executor->login(p_in_msg->user, p_in_msg->password, this->udesc);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::getFileDesc()
	{
		std::shared_ptr<MsgGetFileDesc> p_in_msg = std::static_pointer_cast<MsgGetFileDesc>(this->p_in_msg);
		std::shared_ptr<MsgGetFileDescResp> p_out_msg = std::make_shared<MsgGetFileDescResp>();
		p_out_msg->error_code = p_executor->getFileDesc(p_in_msg->path, p_out_msg->fdesc);
		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::ls()
	{
		std::shared_ptr<MsgLs> p_in_msg = std::static_pointer_cast<MsgLs>(this->p_in_msg);
		std::shared_ptr<MsgLsResp> p_out_msg = std::make_shared<MsgLsResp>();

		Permission perm = Permission::EMPTY;
		ErrorCode ec;
		if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
			p_out_msg->error_code = ec;

		}
		else if (perm & Permission::R) {
			ec = p_executor->ls(p_in_msg->path, p_out_msg->files);
			p_out_msg->error_code = ec;
		}

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::rm()
	{
		std::shared_ptr<MsgRm> p_in_msg = std::static_pointer_cast<MsgRm>(this->p_in_msg);
		std::shared_ptr<MsgRmResp> p_out_msg = std::make_shared<MsgRmResp>();

		Permission perm = Permission::EMPTY;
		ErrorCode ec;
		if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
			p_out_msg->error_code = ec;

		}
		else if ((perm & Permission::W)) {
			ec = p_executor->rm(p_in_msg->path);
			p_out_msg->error_code = ec;

		}
		else {
			p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
		}

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::chown()
	{
		std::shared_ptr<MsgChown> p_in_msg = std::static_pointer_cast<MsgChown>(this->p_in_msg);
		std::shared_ptr<MsgChownResp> p_out_msg = std::make_shared<MsgChownResp>();

		Permission perm = Permission::EMPTY;
		ErrorCode ec;
		if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
			p_out_msg->error_code = ec;

		}
		else if ((perm & Permission::W)) {
			ec = p_executor->rm(p_in_msg->path);
			p_out_msg->error_code = ec;

		}
		else {
			p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
		}

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::chmod()
	{
		std::shared_ptr<MsgChmod> p_in_msg = std::static_pointer_cast<MsgChmod>(this->p_in_msg);
		std::shared_ptr<MsgChmodResp> p_out_msg = std::make_shared<MsgChmodResp>();

		Permission perm = Permission::EMPTY;
		ErrorCode ec;
		if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
			p_out_msg->error_code = ec;

		}
		else if ((perm & Permission::W)) {
			ec = p_executor->chmod(p_in_msg->path, p_in_msg->mod);
			p_out_msg->error_code = ec;

		}
		else {
			p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
		}

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::perm()
	{
		std::shared_ptr<MsgPerm> p_in_msg = std::static_pointer_cast<MsgPerm>(this->p_in_msg);
		std::shared_ptr<MsgPermResp> p_out_msg = std::make_shared<MsgPermResp>();

		do {
			Permission perm = Permission::EMPTY;
			ErrorCode ec;
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
		std::shared_ptr<MsgMkdirResp> p_out_msg = std::make_shared<MsgMkdirResp>();

		do {
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
		std::shared_ptr<MsgOpenResp> p_out_msg = std::make_shared<MsgOpenResp>();
		const std::string& path = p_in_msg->path;
		const std::string& open_mod = p_in_msg->open_mod;

		do {
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

				if (open_mod.find('+') == std::string::npos) {
					p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
					break;
				}

				OpenFileHandler fh;
				if ((ec = p_executor->open(path, open_mod, this->udesc.uid, this->udesc.gid, parent_fdesc, fh))) {
					p_out_msg->error_code = ec;
					break;
				}

				this->open_files[fh.fd] = fh;
				p_out_msg->fd = fh.fd;

			}
			else {
				OpenFileHandler fh;
				if ((ec = p_executor->open(path, open_mod, this->udesc.uid, this->udesc.gid, fdesc, fh))) {
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
		std::shared_ptr<MsgCloseResp> p_out_msg = std::make_shared<MsgCloseResp>();

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
		std::shared_ptr<MsgReadResp> p_out_msg = std::make_shared<MsgReadResp>();
		do {
			if (this->open_files.count(p_in_msg->fd) == 0) {
				p_out_msg->error_code = E_FILE_READ;
				break;
			}

			OpenFileHandler& fh = this->open_files[p_in_msg->fd];
			int32_t max_read_size = std::min(p_in_msg->read_size, p_executor->config.max_msg_size);
			p_out_msg->data = std::string(max_read_size, 0);

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
		std::shared_ptr<MsgWriteResp> p_out_msg = std::make_shared<MsgWriteResp>();

		do {
			if (this->open_files.count(p_in_msg->fd) == 0) {
				p_out_msg->error_code = E_FILE_WRITE;
				p_out_msg->write_size = 0;
				break;
			}

			OpenFileHandler& fh = this->open_files[p_in_msg->fd];
			int32_t write_size = fwrite(p_in_msg->data.c_str(), 1, p_in_msg->data.size(), fh.fp);

			if (write_size != int32_t(p_in_msg->data.size())) {
				p_out_msg->error_code = E_FILE_WRITE;
				p_out_msg->write_size = write_size;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::openOffset()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgOpenOffset> p_in_msg = std::static_pointer_cast<MsgOpenOffset>(this->p_in_msg);
		std::shared_ptr<MsgOpenOffsetResp> p_out_msg = std::make_shared<MsgOpenOffsetResp>();
		const std::string& path = p_in_msg->path;

		do {
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
				if ((ec = p_executor->open(path, "w+", this->udesc.uid, this->udesc.gid, parent_fdesc, fh))) {
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
		std::shared_ptr<MsgReadOffsetResp> p_out_msg = std::make_shared<MsgReadOffsetResp>();

		do {
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

			if (p_in_msg->read_size > p_executor->config.max_msg_size) {
				p_out_msg->error_code = ErrorCode::E_OVERFLOW;
				break;
			}

			FILE* fp = fopen(p_in_msg->path.c_str(), "r");
			if (fp == nullptr) {
				p_out_msg->error_code = ErrorCode::E_FILE_OPEN;
				break;
			}
			if (fseek(fp, p_in_msg->offset, SEEK_SET)) {
				p_out_msg->error_code = ErrorCode::E_FILE_SEEK;
				break;
			}

			p_out_msg->data = std::string(p_in_msg->read_size, 0);
			int32_t read_size = fread(p_out_msg->data.data(), 1, p_in_msg->read_size, fp);

			p_out_msg->data.resize(read_size);

			if (read_size != p_in_msg->read_size) {
				if (feof(fp)) {
					p_out_msg->error_code = ErrorCode::E_FILE_EOF;
				}
				else {
					p_out_msg->error_code = ErrorCode::E_FILE_READ;
				}
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void DataNodeSession::writeOffset()
	{
		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<MsgWriteOffset> p_in_msg = std::static_pointer_cast<MsgWriteOffset>(this->p_in_msg);
		std::shared_ptr<MsgWriteOffsetResp> p_out_msg = std::make_shared<MsgWriteOffsetResp>();

		do {
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

			FILE* fp = fopen(p_in_msg->path.c_str(), "r+");
			if (fp == nullptr) {
				p_out_msg->error_code = ErrorCode::E_FILE_OPEN;
				break;
			}
			if (fseek(fp, p_in_msg->offset, SEEK_SET)) {
				p_out_msg->error_code = ErrorCode::E_FILE_SEEK;
				break;
			}

			int32_t write_size = fwrite(p_in_msg->data.c_str(), 1, p_in_msg->data.size(), fp);

			if (write_size != p_in_msg->data.size()) {
				p_out_msg->error_code = ErrorCode::E_FILE_WRITE;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}
}
