#include <string>

#include "UserDesc.h"

namespace efs {
UserDesc::UserDesc()
{
    user = "";
    password = "";
    uid = -1;
    gid = -1;
}

UserDesc::UserDesc(const UserDesc& udesc)
{
    user = udesc.user;
    password = udesc.password;
    uid = udesc.uid;
    gid = udesc.gid;
}

UserDesc::UserDesc(UserDesc&& udesc)
    : user(std::move(udesc.user))
    , password(std::move(udesc.password))
{
    uid = udesc.uid;
    gid = udesc.gid;
}

UserDesc& UserDesc::operator=(const UserDesc& udesc)
{
    user = udesc.user;
    password = udesc.password;
    uid = udesc.uid;
    gid = udesc.gid;
}

}
