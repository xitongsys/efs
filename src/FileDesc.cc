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

    return Permission(this->mod & 0b111);
}

}