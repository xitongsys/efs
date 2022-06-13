#include <string>

#include "Serialize.h"

namespace efs {

enum FileType : uint8_t {
    FILE,
    DIR
};

enum Permission {
    NONE = 0x0,
    R = 0x1,
    W = 0x2,
    X = 0x4,
};

struct FileDesc {
    std::string path;
    uint64_t fsize;

    uint64_t uid;
    uint64_t gid;
    uint16_t mod;

    inline int32_t size()
    {
        int32_t res = 0;
        res += serialize::size(path);
        res += serialize::size(fsize);
        res += serialize::size(uid);
        res += serialize::size(gid);
        res += serialize::size(mod);
        return res;
    }

    inline int32_t serialize(char* buf, int32_t buf_size)
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

        return size;
    }
};
}