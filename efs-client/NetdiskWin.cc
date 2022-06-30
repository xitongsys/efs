#include <stack>
#include <string>
#include <tuple>

#include "NetdiskWin.h"
#include "FS.h"

namespace efs {

	NetdiskWin::NetdiskWin(std::shared_ptr<Client>& p_client):
		p_client(p_client)
	{
		buffer = new char[EFS_BUFFER_SIZE];
	}

	NetdiskWin::~NetdiskWin()
	{
		delete[] buffer;
	}

	int NetdiskWin::mount(int argc, char* argv[])
	{
		static fuse_operations ops = {
			NetdiskWin::getattr,
			0, //Netdisk::readlink,
			NetdiskWin::mknod,
			NetdiskWin::mkdir,
			NetdiskWin::unlink,
			NetdiskWin::rmdir,
			0, //Netdisk::symlink,
			NetdiskWin::rename,
			0, //Netdisk::link,
			NetdiskWin::chmod,
			NetdiskWin::chown,
			NetdiskWin::truncate,
			NetdiskWin::open,
			NetdiskWin::read,
			NetdiskWin::write,
			NetdiskWin::statfs,
			NetdiskWin::flush,
			NetdiskWin::release,
			0, // fsync
			0, //Netdisk::setxattr,
			0, //Netdisk::getxattr,
			0, //Netdisk::listxattr,
			0, //Netdisk::removexattr,
			NetdiskWin::opendir,
			NetdiskWin::readdir,
			NetdiskWin::releasedir,
			0, // fsyncdir
			NetdiskWin::init,
			0, // destroy
			0, // access
			0, // create
			0, // lock
			NetdiskWin::utimens,
			0, // bmap
		};

		return fuse_main(argc, argv, &ops, this);
	}

	NetdiskWin* NetdiskWin::getself()
	{
		return static_cast<NetdiskWin*>(fuse_get_context()->private_data);
	}

	fuse_timespec NetdiskWin::toFuseTime(int64_t t)
	{
		return fuse_timespec{
			t / 1000,
			(t % 1000) * 1000000
		};
	}

	fuse_stat NetdiskWin::toFuseState(const FileDesc& fdesc)
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

	int NetdiskWin::getattr(const char* path, fuse_stat* stbuf, fuse_file_info* fi)
	{
		//std::cout << "getattr " << path << std::endl;

		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

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

		FileDesc fdesc;
		if ((ec = self->p_client->getFileDesc(path, fdesc))) {
			return -ENOENT;
		}

		*stbuf = toFuseState(fdesc);

		return 0;
	}

	int NetdiskWin::readlink(const char* path, char* buf, size_t size)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);
		return -ENOSYS;
	}

	int NetdiskWin::mknod(const char* path, fuse_mode_t mode, fuse_dev_t dev)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);
		//std::cout << "mknode " << path << std::endl;
		return 0;
	}

	int NetdiskWin::mkdir(const char* path, fuse_mode_t mode)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;

		if ((ec = self->p_client->mkdir(path))) {
			return -EIO;
		}

		return 0;
	}

	int NetdiskWin::unlink(const char* path)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		//std::cout << "unlink " << path << std::endl;
		ErrorCode ec = ErrorCode::NONE;

		if ((ec = self->p_client->rmRecursive(path))) {
			//std::cout << "unlink error: " << int(ec) << std::endl;
			return -EIO;
		}

		return 0;
	}

	int NetdiskWin::rmdir(const char* path)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		//std::cout << "rmdir " << path << std::endl;
		ErrorCode ec = ErrorCode::NONE;
		if ((ec = self->p_client->rmRecursive(path))) {
			return -EIO;
		}

		return 0;
	}

	int NetdiskWin::symlink(const char* dstpath, const char* srcpath)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int NetdiskWin::rename(const char* from_path, const char* to_path, unsigned int flags)
	{
		ErrorCode ec = ErrorCode::NONE;
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		if ((ec = self->p_client->mv(from_path, to_path))) {
			return -EIO;
		}
		return 0;
	}

	int NetdiskWin::link(const char* oldpath, const char* newpath)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		//std::cout << "link " << oldpath << " " << newpath << std::endl;

		return 0;
	}

	int NetdiskWin::chmod(const char* path, fuse_mode_t mode, fuse_file_info* fi)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return -ENOSYS;
	}

	int NetdiskWin::chown(const char* path, fuse_uid_t uid, fuse_gid_t gid, fuse_file_info* fi)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return -ENOSYS;
	}

	int NetdiskWin::truncate(const char* path, fuse_off_t size, fuse_file_info* fi)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);
		//std::cout << "truncat " << path << fi->fh << " " << size << std::endl;

		ErrorCode ec = ErrorCode::NONE;
		if ((ec = self->p_client->truncate(path, size))) {
			//std::cout << "truncate error: " << int(ec) << std::endl;
			return -EIO;
		}

		return 0;
	}

	int NetdiskWin::open(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		/*
		if ((ec = self->p_client->openOffset(path))) {
			return -EIO;
		}
		*/

		int32_t fd = 0;

		//std::cout << "open " << path << " " << fi->flags << std::endl;
		if ((ec = self->p_client->open(path, fi->flags, fd))) {
			//std::cout << "open error: " << int(ec) << std::endl;
			return -EIO;
		}

		//std::cout << "open " << path << " " << fi->flags << " " << fd << std::endl;

		fi->fh = fd;
		return 0;
	}

	int NetdiskWin::read(const char* path, char* buf, size_t size, fuse_off_t off, fuse_file_info* fi)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		/*
		int64_t real_read_size = 0;
		if ((ec = self->p_client->readOffset(path, size, off, buf, real_read_size))) {
			return -EIO;
		}
		*/
		//std::cout << "read " << path << " " << fi->fh << " " << off << " " << size << std::endl;
		std::string data;
		if ((ec = self->p_client->read(path, fi->fh, size, off, data)) && ec != ErrorCode::E_FILE_EOF) {
			//std::cout << "error " << int(ec) << std::endl;
			return -EIO;
		}
		//std::cout << "read size: " << data.size() << " " << data << std::endl;
		memcpy(buf, data.c_str(), data.size());
		return data.size();
	}

	int NetdiskWin::write(const char* path, const char* buf, size_t size, fuse_off_t off, fuse_file_info* fi)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;

		int32_t real_write_size = 0;
		/*
		if ((ec = self->p_client->writeOffset(path, buf, size, off, real_write_size))) {
			return -EIO;
		}
		*/

		//std::cout << "write " << path << " " << fi->fh << " " << off << " " << size << std::endl;

		std::string data(buf, size);
		if ((ec = self->p_client->write(path, fi->fh, data, off, real_write_size))) {
			//std::cout << "error " << int(ec) << std::endl;
			return -EIO;
		}

		//std::cout << "real_write: " << real_write_size << std::endl;

		return real_write_size;
	}

	int NetdiskWin::statfs(const char* path, fuse_statvfs* stbuf)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		// TODO:: change to meaningful value in future
		std::memset(stbuf, 1, sizeof(*stbuf));
		return 0;
	}

	int NetdiskWin::flush(const char* path, fuse_file_info* fi)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return -ENOSYS;
	}

	int NetdiskWin::release(const char* path, fuse_file_info* fi)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		//std::cout << "release " << fi->fh << std::endl;

		if ((ec = self->p_client->close(path, fi->fh))) {
			//std::cout << "error " << int(ec) << std::endl;
			return -EIO;
		}

		return 0;
	}

	int NetdiskWin::setxattr(const char* path, const char* name0, const char* value, size_t size, int flags)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int NetdiskWin::getxattr(const char* path, const char* name0, char* value, size_t size)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int NetdiskWin::listxattr(const char* path, char* namebuf, size_t size)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int NetdiskWin::removexattr(const char* path, const char* name0)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int NetdiskWin::opendir(const char* path, fuse_file_info* fi)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	void* NetdiskWin::init(fuse_conn_info* conn, fuse_config* conf)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		conn->want |= (conn->capable & FUSE_CAP_READDIRPLUS);
		return getself();
	}

	int NetdiskWin::readdir(const char* path, void* buf,
		fuse_fill_dir_t filler,
		fuse_off_t off, fuse_file_info* fi, enum fuse_readdir_flags)
	{
		auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		ErrorCode ec = ErrorCode::NONE;
		std::vector<FileDesc> fdescs;

		//std::cout << "readdir " << path << std::endl;

		//std::cout << "=========" << int(self == nullptr) << std::endl<< int(self->p_client == nullptr) << std::endl;

		if ((ec = self->p_client->ls(path, fdescs))) {
			return -EIO;
		}

		for (auto& fdesc : fdescs) {
			fuse_stat fstat = toFuseState(fdesc);
			if (filler(buf, fs::basename(fdesc.path).c_str(), &fstat, 0, FUSE_FILL_DIR_PLUS)) {
				break;
			}
		}

		return 0;
	}

	int NetdiskWin::releasedir(const char* path, fuse_file_info* fi)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		return 0;
	}

	int NetdiskWin::utimens(const char* path, const fuse_timespec tmsp[2], fuse_file_info* fi)
	{
		//auto self = getself();
		//std::lock_guard<std::mutex> lock(self->mutex);

		//std::cout << "utimens" << std::endl;

		return 0;
	}
}
