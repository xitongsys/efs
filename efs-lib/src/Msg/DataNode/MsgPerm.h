#pragma once

#include <string>
#include <vector>

#include "FileDesc.h"
#include "Msg/Msg.h"
#include "Serialize.h"

namespace efs {
struct MsgPerm : Msg {
    std::string path;
    std::string name;

    // USER/GROUP/OTHER
    PermType perm_type;

    // > 0: add permission
    // < 0: remove permission
    Permission perm;

    MsgPerm()
    {
        msg_type = MsgType::PERM;
        error_code = ErrorCode::NONE;
        path = "";
        name = "";
        perm_type = PermType::USER;
        perm = Permission::EMPTY;
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(path) + serialize::size(name) + serialize::size(perm_type) + serialize::size(perm);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(path, buf + size, buf_size - size);
        size += serialize::serialize(name, buf + size, buf_size - size);
        size += serialize::serialize(perm_type, buf + size, buf_size - size);
        size += serialize::serialize(perm, buf + size, buf_size - size);
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

        if ((size1 = serialize::deserialize(name, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(perm_type, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(perm, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};

struct MsgPermResp : Msg {

    MsgPermResp()
    {
        msg_type = MsgType::PERM_RESP;
        error_code = ErrorCode::NONE;
    }
};
}