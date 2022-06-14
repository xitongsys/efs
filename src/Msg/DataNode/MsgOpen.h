#include <string>
#include <vector>

#include "FileDesc.h"
#include "Msg/Msg.h"
#include "Serialize.h"

namespace efs {
struct MsgOpen : Msg {
    std::string path;
    std::string open_mod;

    MsgOpen()
    {
        msg_type = MsgType::OPEN;
        path = "";
        open_mod = "";
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(path) + serialize::size(open_mod);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(path, buf + size, buf_size - size);
        size += serialize::serialize(open_mod, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        if ((size1 = Msg::deserialize(buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(path, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(open_mod, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};

struct MsgOpenResp : Msg {
    int32_t fd;

    MsgOpenResp()
    {
        msg_type = MsgType::OPEN_RESP;
        error_code = ErrorCode::NONE;
        fd = 0;
    }

    inline int32_t size() const
    {
        int32_t size = Msg::size();
        size += serialize::size(fd);
        return size;
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        int32_t size = 0;
        if (this->size() > buf_size) {
            return -1;
        }
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(fd, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(char* buf, int32_t buf_size)
    {
        int32_t size = 0, size1 = 0;
        if ((size1 = Msg::deserialize(buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(fd, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}