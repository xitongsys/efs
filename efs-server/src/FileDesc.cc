#include <string>

#include "FileDesc.h"
#include "Serialize.h"
#include "Util.h"

namespace efs {

	FileDesc::FileDesc()
	{
		path = "";
		fsize = 0;
		uid = 0;
		gid = 0;
		mod = 0700;
		user_perms = {};
		group_perms = {};
		create_time = util::now();
		modified_time = util::now();
	}

	FileDesc::FileDesc(const FileDesc& fdesc)
	{
		path = fdesc.path;
		fsize = fdesc.fsize;
		uid = fdesc.uid;
		gid = fdesc.gid;
		mod = fdesc.mod;
		user_perms = fdesc.user_perms;
		group_perms = fdesc.group_perms;
		create_time = fdesc.create_time;
		modified_time = fdesc.modified_time;
	}

	FileDesc::FileDesc(FileDesc&& fdesc)
		: path(std::move(fdesc.path))
	{
		fsize = fdesc.fsize;
		uid = fdesc.uid;
		gid = fdesc.gid;
		mod = fdesc.mod;
		user_perms = fdesc.user_perms;
		group_perms = fdesc.group_perms;
		create_time = fdesc.create_time;
		modified_time = fdesc.modified_time;
	}

	FileDesc& FileDesc::operator=(const FileDesc& fdesc)
	{
		path = fdesc.path;
		fsize = fdesc.fsize;
		uid = fdesc.uid;
		gid = fdesc.gid;
		mod = fdesc.mod;
		user_perms = fdesc.user_perms;
		group_perms = fdesc.group_perms;
		create_time = fdesc.create_time;
		modified_time = fdesc.modified_time;

		return *this;
	}

	Permission FileDesc::perm(uint16_t uid, uint16_t gid) const
	{
		// for root user and root group
		if (uid == 1 || gid == 1) {
			return Permission(0b111);
		}

		// user perm has higher priority
		if (this->uid == uid) {
			return getUserPerm(this->mod);
		}

		if (this->gid == gid) {
			return getGroupPerm(this->mod);
		}

		for (auto& id_perm : user_perms) {
			if (id_perm.id == uid) {
				return getUserPerm(id_perm.perm);
			}
		}

		for (auto& id_perm : group_perms) {
			if (id_perm.id == gid) {
				return getGroupPerm(id_perm.perm);
			}
		}

		return getOtherPerm(this->mod);
	}

}