#pragma once

#include <string>
#include <vector>

#include "FileDesc.h"
#include "Msg/Msg.h"
#include "Serialize.h"

namespace efs {
struct MsgLogin : Msg {
    std::string user, password;

    MsgLogin()
    {
        msg_type = MsgType::LOGIN;
        user = "";
        password = "";
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(user) + serialize::size(password);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(user, buf + size, buf_size - size);
        size += serialize::serialize(password, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        size1 = Msg::deserialize(buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(user, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(password, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};

struct MsgLoginResp : Msg {
    MsgLoginResp()
    {
        msg_type = MsgType::LOGIN_RESP;
        error_code = ErrorCode::NONE;
    }
};
}