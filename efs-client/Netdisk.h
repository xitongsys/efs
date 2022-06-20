#pragma once

#include <stdint.h>
#include <memory>
#include <unordered_map>
#include <fuse3/fuse.h>

#include "Client.h"


namespace efs {
	class Netdisk {
	public:
		static std::shared_ptr<Client> p_client;
		static std::unordered_map<std::string, int32_t> open_fds;

	public:
		Netdisk(std::shared_ptr<Client> p_client);
		int mount(int argc, char* argv[]);

		static Netdisk* getself();
		static fuse_timespec toFuseTime(int64_t t);
		static fuse_stat toFuseState(const FileDesc& fdesc);
		static std::shared_ptr<DataNodeConn> getConn(const std::string& path);

		static int getattr(const char* path, struct fuse_stat* stbuf, struct fuse_file_info* fi);
		static int readlink(const char* path, char* buf, size_t size);
		static int mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev);
		static int mkdir(const char* path, fuse_mode_t mode);
		static int unlink(const char* path);
		static int rmdir(const char* path);
		static int symlink(const char* dstpath, const char* srcpath);
		static int rename(const char* oldpath, const char* newpath, unsigned int flags);
		static int link(const char* oldpath, const char* newpath);
		static int chmod(const char* path, fuse_mode_t mode, struct fuse_file_info* fi);
		static int chown(const char* path, fuse_uid_t uid, fuse_gid_t gid, struct fuse_file_info* fi);
		static int truncate(const char* path, fuse_off_t size, struct fuse_file_info* fi);
		static int open(const char* path, struct fuse_file_info* fi);
		static int read(const char* path, char* buf, size_t size, fuse_off_t off, struct fuse_file_info* fi);
		static int write(const char* path, const char* buf, size_t size, fuse_off_t off, struct fuse_file_info* fi);
		static int statfs(const char* path, struct fuse_statvfs* stbuf);
		static int flush(const char* path, struct fuse_file_info* fi);
		static int release(const char* path, struct fuse_file_info* fi);
		static int setxattr(const char* path, const char* name0, const char* value, size_t size, int flags);
		static int getxattr(const char* path, const char* name0, char* value, size_t size);
		static int listxattr(const char* path, char* namebuf, size_t size);
		static int removexattr(const char* path, const char* name0);
		static int opendir(const char* path, struct fuse_file_info* fi);
		static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, fuse_off_t off, struct fuse_file_info* fi, enum fuse_readdir_flags);
		static int releasedir(const char* path, struct fuse_file_info* fi);
		static int utimens(const char* path, const struct fuse_timespec tmsp[2], struct fuse_file_info* fi);
		static void* init(struct fuse_conn_info* conn, struct fuse_config* conf);
	};

}

