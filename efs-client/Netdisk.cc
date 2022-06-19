#include "Netdisk.h"

namespace efs {
	Netdisk::Netdisk(std::shared_ptr<Client> p_client) :
		p_client(p_client)
	{
	}

	ErrorCode Netdisk::mount()
	{
		return ErrorCode();
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
		fuse_stat stat;
		stat.st_ino = 1;
		stat.st_mode = fdesc.mod;
		stat.st_nlink = 1;
		stat.st_uid = fdesc.uid;
		stat.st_gid = fdesc.gid;
		stat.st_rdev = 0;
		stat.st_ctim = toFuseTime(fdesc.create_time);
		stat.st_mtim = toFuseTime(fdesc.modified_time);
		stat.st_atim = toFuseTime(fdesc.modified_time);
		return stat;
	}

	std::shared_ptr<DataNodeConn> Netdisk::getConn(const std::string& path)
	{
		std::shared_ptr<DataNodeConn> p_conn = nullptr;
		p_client->getDataNodeConn(path, p_conn);
		return p_conn;
	}

	int Netdisk::getattr(const char* path, fuse_stat* stbuf, fuse_file_info* fi)
	{
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		FileDesc fdesc;
		if (p_conn->getFileDesc(path, fdesc)) {
			return -ENOENT;
		}

		stbuf->st_ino = 1;
		stbuf->st_mode = fdesc.mod;
		stbuf->st_nlink = 1;
		stbuf->st_uid = fdesc.uid;
		stbuf->st_gid = fdesc.gid;
		stbuf->st_rdev = 0;
		stbuf->st_ctim = toFuseTime(fdesc.create_time);
		stbuf->st_mtim = toFuseTime(fdesc.modified_time);
		stbuf->st_atim = toFuseTime(fdesc.modified_time);

		return 0;
	}

	int Netdisk::readlink(const char* path, char* buf, size_t size)
	{
		return -ENOSYS;
	}

	int Netdisk::mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev)
	{
		return -ENOSYS;
	}

	int Netdisk::mkdir(const char* path, fuse_mode_t mode)
	{
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		if (p_conn->mkdir(path)) {
			return -ENOENT;
		}
		return 0;
	}

	int Netdisk::unlink(const char* path)
	{
		return -ENOSYS;
	}

	int Netdisk::rmdir(const char* path)
	{
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
		return -ENOSYS;
	}

	int Netdisk::rename(const char* oldpath, const char* newpath, unsigned int flags)
	{
		return -ENOSYS;
	}

	int Netdisk::link(const char* oldpath, const char* newpath)
	{
		return -ENOSYS;
	}

	int Netdisk::chmod(const char* path, fuse_mode_t mode, fuse_file_info* fi)
	{
		return -ENOSYS;
	}

	int Netdisk::chown(const char* path, fuse_uid_t uid, fuse_gid_t gid, fuse_file_info* fi)
	{
		return -ENOSYS;
	}

	int Netdisk::truncate(const char* path, fuse_off_t size, fuse_file_info* fi)
	{
		return -ENOSYS;
	}

	int Netdisk::open(const char* path, fuse_file_info* fi)
	{
		if (open_fds.count(path)) {
			return 0;
		}

		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		int32_t fd = 0;
		if (p_conn->open(path, "wr", fd)) {
			return -ENOENT;
		}

		open_fds[path] = fd;
		fi->fh = fd;

		return 0;
	}

	int Netdisk::read(const char* path, char* buf, size_t size, fuse_off_t off, fuse_file_info* fi)
	{
		if (!open_fds.count(path)) {
			return -ENOENT;
		}

		int32_t fd = open_fds[path];
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		std::string data;
		if (p_conn->read(fd, size, data)) {
			return -ENOENT;
		}

		std::memcpy(buf, data.c_str(), data.size());
		return data.size();		
	}

	int Netdisk::write(const char* path, const char* buf, size_t size, fuse_off_t off, fuse_file_info* fi)
	{
		if (!open_fds.count(path)) {
			return -ENOENT;
		}

		int32_t fd = open_fds[path];
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		int32_t write_size = 0;
		std::string data(buf, size);
		if (p_conn->write(fd, data, write_size)) {
			return -ENOENT;
		}

		return write_size;
	}

	int Netdisk::statfs(const char* path, fuse_statvfs* stbuf)
	{
		std::memset(stbuf, 0, sizeof * stbuf);
		return 0;
	}

	int Netdisk::flush(const char* path, fuse_file_info* fi)
	{
		return -ENOSYS;
	}

	int Netdisk::release(const char* path, fuse_file_info* fi)
	{
		return 0;
	}

	int Netdisk::setxattr(const char* path, const char* name0, const char* value, size_t size, int flags)
	{
		return -ENOSYS;
	}

	int Netdisk::getxattr(const char* path, const char* name0, char* value, size_t size)
	{
		return -ENOSYS;
	}

	int Netdisk::listxattr(const char* path, char* namebuf, size_t size)
	{
		return -ENOSYS;
	}

	int Netdisk::removexattr(const char* path, const char* name0)
	{
		return -ENOSYS;
	}

	int Netdisk::opendir(const char* path, fuse_file_info* fi)
	{
		return 0;
	}

	int Netdisk::readdir(const char* path, void* buf, fuse_fill_dir_t filler, fuse_off_t off, fuse_file_info* fi, fuse_readdir_flags)
	{
		std::shared_ptr<DataNodeConn> p_conn = getConn(path);
		if (p_conn == nullptr) {
			return -ENOENT;
		}

		std::vector<FileDesc> fdescs;
		if (p_conn->ls(path, fdescs)) {
			return -ENOENT;
		}

		for (auto& fdesc : fdescs) {
			fuse_stat fstat = toFuseState(fdesc);
			if (filler(buf, fdesc.path.c_str(), &fstat, 0, FUSE_FILL_DIR_PLUS)) {
				break;
			}
		}
		return 0;
	}

	int Netdisk::releasedir(const char* path, fuse_file_info* fi)
	{
		return 0;
	}

	int Netdisk::utimens(const char* path, const fuse_timespec tmsp[2], fuse_file_info* fi)
	{
		return 0;
	}
}
