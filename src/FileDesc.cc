#include <string>

#include "FileDesc.h"
#include "Serialize.h"
#include "Util.h"

namespace efs {

FileDesc::FileDesc()
{
    path = "";
    fsize = 0;
    uid = -1;
    gid = -1;
    mod = 0b0111000000;
    perms = {};
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
    perms = fdesc.perms;
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
    perms = fdesc.perms;
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
    perms = fdesc.perms;
    create_time = fdesc.create_time;
    modified_time = fdesc.modified_time;

    return *this;
}

Permission FileDesc::perm(int32_t uid, int64_t gid) const
{
    if (this->uid == uid) {
        return Permission((this->mod >> 6) & (0b111));
    }

    if (this->gid == gid) {
        return Permission((this->mod >> 3) & (0b111));
    }

    for (auto& id_perm : perms) {
        int16_t id = id_perm >> 16;
        Permission perm = Permission(id_perm & 0b111);
        IdType id_type = IdType((id_perm >> 4) & 1);

        if (id_type == IdType::USER && id == uid) {
            return perm;
        } else if (id_type == IdType::GROUP && id == gid) {
            return perm;
        }
    }

    return Permission(this->mod & 0b111);
}

}