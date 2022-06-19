#pragma once

#include <stdint.h>
#include <memory>
#include <fuse3/fuse.h>

#include "Client.h"


namespace efs {
	class Netdisk {
	public:
		std::shared_ptr<Client> p_client;

	public:
		Netdisk(std::shared_ptr<Client> p_client);

		ErrorCode mount();

		fuse_timespec toFuseTime(int64_t t);
		std::shared_ptr<DataNodeConn> getConn(const std::string& path);

		int getattr(const char* path, struct fuse_stat* stbuf, struct fuse_file_info* fi);
		int readlink(const char* path, char* buf, size_t size);
		int mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev);
		int mkdir(const char* path, fuse_mode_t mode);
		int unlink(const char* path);
		int rmdir(const char* path);
		int symlink(const char* dstpath, const char* srcpath);
		int rename(const char* oldpath, const char* newpath, unsigned int flags);
		int link(const char* oldpath, const char* newpath);
		int chmod(const char* path, fuse_mode_t mode, struct fuse_file_info* fi);
		int chown(const char* path, fuse_uid_t uid, fuse_gid_t gid, struct fuse_file_info* fi);
		int truncate(const char* path, fuse_off_t size, struct fuse_file_info* fi);
		int open(const char* path, struct fuse_file_info* fi);
		int read(const char* path, char* buf, size_t size, fuse_off_t off, struct fuse_file_info* fi);
		int write(const char* path, const char* buf, size_t size, fuse_off_t off, struct fuse_file_info* fi);
		int statfs(const char* path, struct fuse_statvfs* stbuf);
		int flush(const char* path, struct fuse_file_info* fi);
		int release(const char* path, struct fuse_file_info* fi);
		int setxattr(const char* path, const char* name0, const char* value, size_t size, int flags);
		int getxattr(const char* path, const char* name0, char* value, size_t size);
		int listxattr(const char* path, char* namebuf, size_t size);
		int removexattr(const char* path, const char* name0);
		int opendir(const char* path, struct fuse_file_info* fi);
		int readdir(const char* path, void* buf, fuse_fill_dir_t filler, fuse_off_t off, struct fuse_file_info* fi, enum fuse_readdir_flags);
		int releasedir(const char* path, struct fuse_file_info* fi);
		int utimens(const char* path, const struct fuse_timespec tmsp[2], struct fuse_file_info* fi);
	};

}

