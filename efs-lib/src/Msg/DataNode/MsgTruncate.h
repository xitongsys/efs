#pragma once

#include <string>

#include "Msg/Msg.h"

namespace efs {

struct MsgTruncate : Msg {
    std::string path;
    int64_t offset;

    MsgTruncate()
    {
        msg_type = MsgType::TRUNCATE;
        error_code = ErrorCode::NONE;

        path = "";
        offset = 0;
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(path) + serialize::size(offset);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(path, buf + size, buf_size - size);
        size += serialize::serialize(offset, buf + size, buf_size - size);
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

        if ((size1 = serialize::deserialize(offset, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};

struct MsgTruncateResp : Msg {

    MsgTruncateResp()
    {
        msg_type = MsgType::TRUNCATE_RESP;
        error_code = ErrorCode::NONE;
    }
};
}