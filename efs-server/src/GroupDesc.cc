#include <string>

#include "GroupDesc.h"
#include "Util.h"

namespace efs {
GroupDesc::GroupDesc()
{
    group = "";
    gid = -1;
}

GroupDesc::GroupDesc(const std::string& csv)
{
    std::vector<std::string> vs = util::split(csv, ',');
    if (vs.size() != 2) {
        throw ErrorCode::E_PANIC;
    }

    group = vs[0];
    gid = std::stoi(vs[1]);
}

GroupDesc::GroupDesc(const GroupDesc& gdesc)
{
    group = gdesc.group;
    gid = gdesc.gid;
}

GroupDesc::GroupDesc(GroupDesc&& gdesc)
    : group(std::move(gdesc.group))
{
    gid = gdesc.gid;
}

GroupDesc& GroupDesc::operator=(const GroupDesc& gdesc)
{
    group = gdesc.group;
    gid = gdesc.gid;

    return *this;
}

}
