#include <string>

#include "UserDesc.h"
#include "Util.h"

namespace efs {
UserDesc::UserDesc()
{
    user = "";
    password = "";
    uid = -1;
    gid = -1;
}

UserDesc::UserDesc(const std::string& csv)
{
    std::vector<std::string> vs = util::split(csv, ',');
    if (vs.size() != 4) {
        throw ErrorCode::E_PANIC;
    }

    user = vs[0];
    password = vs[1];
    uid = std::stoi(vs[2]);
    gid = std::stoi(vs[3]);
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
