#pragma once

#include <string>

#include "Serialize.h"

namespace efs {
struct GroupDesc {
    std::string group;
    uint16_t gid;

    GroupDesc();

    GroupDesc(const std::string& csv);

    GroupDesc(const GroupDesc& gdesc);

    GroupDesc(GroupDesc&& gdesc);

    GroupDesc& operator=(const GroupDesc& gdesc);

    inline int32_t size() const
    {
        int32_t res = 0;
        res += serialize::size(group);
        res += serialize::size(gid);
        return res;
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += serialize::serialize(group, buf + size, buf_size - size);
        size += serialize::serialize(gid, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;

        if ((size1 = serialize::deserialize(group, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(gid, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}
