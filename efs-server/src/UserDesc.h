#pragma once

#include <string>

#include "Serialize.h"

namespace efs {

struct UserDesc {
    std::string user;
    std::string password;
    uint16_t uid;
    uint16_t gid;
    std::string root_path;

    UserDesc();

    UserDesc(const std::string& csv);

    UserDesc(const UserDesc& udesc);

    UserDesc(UserDesc&& udesc);

    UserDesc& operator=(const UserDesc& udesc);

    inline int32_t size() const
    {
        int32_t res = 0;
        res += serialize::size(user);
        res += serialize::size(password);
        res += serialize::size(uid);
        res += serialize::size(gid);
        res += serialize::size(root_path);
        return res;
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += serialize::serialize(user, buf + size, buf_size - size);
        size += serialize::serialize(password, buf + size, buf_size - size);
        size += serialize::serialize(uid, buf + size, buf_size - size);
        size += serialize::serialize(gid, buf + size, buf_size - size);
        size += serialize::serialize(root_path, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;

        if ((size1 = serialize::deserialize(user, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(password, buf + size, buf_size - size)) < 0) {
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

        if ((size1 = serialize::deserialize(root_path, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}
