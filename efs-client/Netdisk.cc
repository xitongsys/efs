#include "Netdisk.h"
#include "compat.h"
#include "FS.h"

namespace efs {

	std::shared_ptr<Client> Netdisk::p_client = nullptr;

	Netdisk::Netdisk(std::shared_ptr<Client> p_client)
	{
		Netdisk::p_client = p_client;
	}

	int Netdisk::mount(int argc, char* argv[])
	{
		static fuse_operations ops = {
			Netdisk::getattr,
			0, //Netdisk::readlink,
			Netdisk::mknod,
			Netdisk::mkdir,
			Netdisk::unlink,
			Netdisk::rmdir,
			0, //Netdisk::symlink,
			Netdisk::rename,
			0, //Netdisk::link,
			Netdisk::chmod,
			Netdisk::chown,
			Netdisk::truncate,
			Netdisk::open,
			Netdisk::read,
			Netdisk::write,
			Netdisk::statfs,
			Netdisk::flush,
			Netdisk::release,
			0, // fsync
			0, //Netdisk::setxattr,
			0, //Netdisk::getxattr,
			0, //Netdisk::listxattr,
			0, //Netdisk::removexattr,
			Netdisk::opendir,
			Netdisk::readdir,
			Netdisk::releasedir,
			0, // fsyncdir
			Netdisk::init,
			0, // destroy
			0, // access
			0, // create
			0, // lock
			Netdisk::utimens,
			0, // bmap
		};

		return fuse_main(argc, argv, &ops, this);


	}

	Netdisk* Netdisk::getself()
	{
		return static_cast<Netdisk*>(fuse_get_context()->private_data);
	}

	fuse_timespec Netdisk::toFuseTime(int64_t t)
	{
		return fuse_timespec{
			t / 1000,
			(t % 1000) * 1000000
		};
	}

	fuse_stat Netdisk::toFuseState(const FileDesc& fdesc)
	{
		fuse_context* context = fuse_get_context();
		fuse_stat stat;
		stat.st_ino = 1;
		stat.st_mode = fdesc.mod | 0777;
		stat.st_nlink = 1;
		stat.st_uid = context->uid;
		stat.st_gid = context->gid;
		stat.st_rdev = 0;
		stat.st_size = fdesc.fsize;
		stat.st_ctim = toFuseTime(fdesc.create_time);
		stat.st_mtim = toFuseTime(fdesc.modified_time);
		stat.st_atim = toFuseTime(fdesc.modified_time);
		return stat;
	}

	std::shared_ptr<DataNodeConn> Netdisk::getConn(const std::string& path)
	{
		std::shared_ptr<DataNodeConn> p_conn = nullptr;
		Netdisk::p_client->getDataNodeConn(path, p_conn);
		return p_conn;
	}

	int Netdisk::getattr(const char* path, fuse_stat* stbuf, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		//TODO:: maybe change more elegant
		if (strcmp(path, "/") == 0) {
			fuse_context* context = fuse_get_context();
			stbuf->st_ino = 1;
			stbuf->st_mode = 040777;
			stbuf->st_nlink = 1;
			stbuf->st_uid = context->uid;
			stbuf->st_gid = context->gid;
			stbuf->st_rdev = 0;
			stbuf->st_ctim = toFuseTime(0);
			stbuf->st_mtim = toFuseTime(0);
			stbuf->st_atim = toFuseTime(0);
			return 0;
		}

		std::shared_ptr<DataNodeConn> p_conn = Netdisk::getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		FileDesc fdesc;
		if ((ec = p_conn->getFileDesc(path, fdesc))) {
			return -ENOENT;
		}

		*stbuf = toFuseState(fdesc);

		return 0;
	}

	int Netdisk::readlink(const char* path, char* buf, size_t size)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);
		return -ENOSYS;
	}

	int Netdisk::mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);
		return 0;
	}

	int Netdisk::mkdir(const char* path, fuse_mode_t mode)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::shared_ptr<DataNodeConn> p_conn = getConn(path);

		if (p_conn == nullptr) {
			return -ENOENT;
		}

		if (p_conn->mkdir(path)) {
			return -EIO;
		}
		return 0;
	}

	int Netdisk::unlink(const char* path)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::shared_ptr<DataNodeConn> p_conn = getConn(path);

		if (p_conn == nullptr) {
			return -ENOENT;
		}

		if (p_conn->rm(path)) {
			return -EIO;
		}

		return 0;
	}

	int Netdisk::rmdir(const char* path)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		if (p_conn->rm(path)) {
			return -ENOENT;
		}
		return 0;
	}

	int Netdisk::symlink(const char* dstpath, const char* srcpath)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::rename(const char* oldpath, const char* newpath, unsigned int flags)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return -ENOSYS;
	}

	int Netdisk::link(const char* oldpath, const char* newpath)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::chmod(const char* path, fuse_mode_t mode, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return -ENOSYS;
	}

	int Netdisk::chown(const char* path, fuse_uid_t uid, fuse_gid_t gid, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::truncate(const char* path, fuse_off_t size, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		if ((ec = p_conn->truncate(path, size))) {
			return -EIO;
		}

		return 0;
	}

	int Netdisk::open(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::shared_ptr<DataNodeConn> p_conn = getConn(path);

		if (p_conn == nullptr) {
			return -ENOENT;
		}

		if (p_conn->openOffset(path)) {
			return -ENOENT;
		}

		return 0;
	}

	int Netdisk::read(const char* path, char* buf, size_t size, fuse_off_t off, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		int64_t i = 0;
		std::string data;

		while (i < size) {
			int64_t rs = std::min(size - i, size_t(EFS_MAX_READ_SIZE));
			int32_t real_read_size = 0;
			ErrorCode ec = p_conn->readOffset(path, rs, off + i, buf + i, real_read_size);
			if (ec && ec != ErrorCode::E_FILE_EOF) {
				return -ENOENT;
			}

			i += real_read_size;
			if (ec == ErrorCode::E_FILE_EOF) {
				break;
			}
		}

		return i;
	}

	int Netdisk::write(const char* path, const char* buf, size_t size, fuse_off_t off, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		int64_t i = 0;
		while (i < size) {
			int32_t ws = std::min(size - i, size_t(EFS_MAX_READ_SIZE));
			int32_t real_write_size = 0;
			if ((ec = p_conn->writeOffset(path, buf + i, ws, off + i, real_write_size))) {
				return -EIO;
			}
			i += real_write_size;
		}

		return i;
	}

	int Netdisk::statfs(const char* path, fuse_statvfs* stbuf)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::memset(stbuf, 0, sizeof(*stbuf));
		return 0;
	}

	int Netdisk::flush(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return -ENOSYS;
	}

	int Netdisk::release(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::setxattr(const char* path, const char* name0, const char* value, size_t size, int flags)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::getxattr(const char* path, const char* name0, char* value, size_t size)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::listxattr(const char* path, char* namebuf, size_t size)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::removexattr(const char* path, const char* name0)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::opendir(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	void* Netdisk::init(fuse_conn_info* conn, fuse_config* conf)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		conn->want |= (conn->capable & FUSE_CAP_READDIRPLUS);
		return getself();
	}

	int Netdisk::readdir(const char* path, void* buf,
		fuse_fill_dir_t filler,
		fuse_off_t off, fuse_file_info* fi, enum fuse_readdir_flags)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		std::vector<std::shared_ptr<DataNodeConn>> p_conns;
		if (strcmp(path, "/") == 0) {
			p_client->getAllDataNodeConns(p_conns);
		}
		else {
			std::shared_ptr<DataNodeConn> p_conn = getConn(path);
			if (p_conn) {
				p_conns.push_back(p_conn);
			}
		}

		if (p_conns.size() == 0) {
			return -ENOENT;
		}

		for (auto& p_conn : p_conns) {
			std::vector<FileDesc> fdescs;
			if (p_conn->ls(path, fdescs)) {
				return -ENOENT;
			}

			for (auto& fdesc : fdescs) {
				fuse_stat fstat = toFuseState(fdesc);
				if (filler(buf, fs::basename(fdesc.path).c_str(), &fstat, 0, FUSE_FILL_DIR_PLUS)) {
					break;
				}
			}
		}
		return 0;
	}

	int Netdisk::releasedir(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int Netdisk::utimens(const char* path, const fuse_timespec tmsp[2], fuse_file_info* fi)
	{
		auto self = getself();
		std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}
}
