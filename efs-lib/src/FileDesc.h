#pragma once

#include <string>

#include "Serialize.h"
#include "Util.h"

namespace efs {

	enum Permission : uint8_t {
		EMPTY = 0b0,
		R = 0b100,
		W = 0b010,
		X = 0b001,
	};

	enum PermType : uint8_t {
		DEFAULT_PERMTYPE = 0,
		USER = 1,
		GROUP = 2,
		OTHER = 3,
	};

	enum FileType : uint16_t {
		F_IFMT = 0170000,
		F_IFSOCK = 0140000,  // socket
		F_IFLNK = 0120000,   //symbolic link
		F_IFREG = 0100000,   //regular file
		F_IFBLK = 0060000,   //block device
		F_IFDIR = 0040000,   //directory
		F_IFCHR = 0020000,   //character device
		F_IFIFO = 0010000,   //FIFO
	};

	enum OpenFlag : uint32_t {
		O_RDONLY = 0,
		O_WRONLY = 1,
		O_RDWR = 2,
		O_ACCMODE = 3,
	};

	inline std::string openFlagToMode(const OpenFlag& flag)
	{
		if ((flag & OpenFlag::O_ACCMODE) == 0) {
			return "rb";
		}
		return "rb+";
	}

	inline Permission getUserPerm(uint16_t mod) {
		return Permission((mod >> 6) & 0b111);
	}

	inline Permission getGroupPerm(uint16_t mod) {
		return Permission((mod >> 3) & 0b111);
	}

	inline Permission getOtherPerm(uint16_t mod) {
		return Permission((mod >> 0) & 0b111);
	}

	inline FileType getFileType(uint16_t mod) {
		return FileType(mod & F_IFMT);
	}

	inline void setUserPerm(uint16_t& mod, Permission p) {
		mod = (mod & (~(0b111000000))) | (p << 6);
	}

	inline void setGroupPerm(uint16_t& mod, Permission p) {
		mod = (mod & (~(0b000111000))) | (p << 3);
	}

	inline void setOtherPerm(uint16_t& mod, Permission p) {
		mod = (mod & (~(0b000000111))) | (p << 0);
	}

	inline void setFileType(uint16_t& mod, int16_t ft) {
		mod = (mod & (~(0b1111000000000000))) | ft;
	}

	struct IdPerm {
		uint16_t id;
		uint16_t perm;

		inline int32_t size() const {
			return serialize::size(id) + serialize::size(perm);
		}

		inline int32_t serialize(char* buf, int32_t buf_size) const
		{
			if (this->size() > buf_size) {
				return -1;
			}
			int32_t size = 0;
			size += serialize::serialize(id, buf + size, buf_size - size);
			size += serialize::serialize(perm, buf + size, buf_size - size);
			return size;
		}

		inline int32_t deserialize(const char* buf, int32_t buf_size)
		{
			int size = 0, size1 = 0;

			if ((size1 = serialize::deserialize(id, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(perm, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			return size;
		}
	};

	struct FileDesc {
		std::string path;
		int64_t fsize;

		uint16_t uid;
		uint16_t gid;
		uint16_t mod;

		// perm: High id(uint16_t) + (int16_t)mod Low
		std::vector<IdPerm> user_perms;
		std::vector<IdPerm> group_perms;
		int64_t create_time;
		int64_t modified_time;

		FileDesc();
		FileDesc(const FileDesc& fdesc);
		FileDesc(FileDesc&& fdesc);
		FileDesc& operator=(const FileDesc& fdesc);
		Permission perm(uint16_t uid, uint16_t gid) const;

		inline int32_t size() const
		{
			int32_t res = 0;
			res += serialize::size(path);
			res += serialize::size(fsize);
			res += serialize::size(uid);
			res += serialize::size(gid);
			res += serialize::size(mod);
			res += serialize::size(user_perms);
			res += serialize::size(group_perms);
			res += serialize::size(create_time);
			res += serialize::size(modified_time);
			return res;
		}

		inline int32_t serialize(char* buf, int32_t buf_size) const
		{
			if (this->size() > buf_size) {
				return -1;
			}
			int32_t size = 0;
			size += serialize::serialize(path, buf + size, buf_size - size);
			size += serialize::serialize(fsize, buf + size, buf_size - size);
			size += serialize::serialize(uid, buf + size, buf_size - size);
			size += serialize::serialize(gid, buf + size, buf_size - size);
			size += serialize::serialize(mod, buf + size, buf_size - size);
			size += serialize::serialize(user_perms, buf + size, buf_size - size);
			size += serialize::serialize(group_perms, buf + size, buf_size - size);
			size += serialize::serialize(create_time, buf + size, buf_size - size);
			size += serialize::serialize(modified_time, buf + size, buf_size - size);
			return size;
		}

		inline int32_t deserialize(const char* buf, int32_t buf_size)
		{
			int size = 0, size1 = 0;

			if ((size1 = serialize::deserialize(path, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(fsize, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(uid, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(gid, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(mod, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(user_perms, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(group_perms, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(create_time, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(modified_time, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			return size;
		}
	};
}