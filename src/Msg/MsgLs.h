#include <string>
#include <vector>

#include "FileDesc.h"
#include "Msg/Msg.h"
#include "Serialize.h"

namespace efs {
struct MsgLs : Msg {
    std::string path;

    // control block, don't serialize
    bool is_done;

    MsgLs()
    {
        path = "";
    }

    int32_t size()
    {
        return Msg::size() + serialize::size(path);
    }

    int32_t serialize(char* buf, int32_t buf_size)
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(path, buf + size, buf_size - size);
        return size;
    }

    int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        size1 = Msg::deserialize(buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        size += serialize::deserialize(path, buf + size, buf_size - size);
        return size;
    }
};

struct MsgLsResp : Msg {
    std::vector<FileDesc> files;

    MsgLsResp()
    {
    }

    int32_t size()
    {
        int32_t size = Msg::size();
        size += serialize::size(files);
        return size;
    }

    int32_t serialize(char* buf, int32_t buf_size)
    {
        int32_t size = 0;
        if (this->size() > buf_size) {
            return -1;
        }
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(files, buf + size, buf_size - size);
        return size;
    }

    int32_t deserialize(char* buf, int32_t buf_size)
    {
        int32_t size = 0, size1 = 0;
        size1 = Msg::deserialize(buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        size1 = serialize::deserialize(files, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}