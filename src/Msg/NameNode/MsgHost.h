#pragma once

#include <string>
#include <vector>

#include "GroupDesc.h"
#include "HostDesc.h"
#include "Msg/Msg.h"
#include "UserDesc.h"

namespace efs {

struct MsgHost : Msg {
    MsgHost()
    {
        msg_type = MsgType::HOST;
        error_code = ErrorCode::NONE;
    }
};

struct MsgHostResp : Msg {
    std::vector<HostDesc> hosts;

    MsgHostResp()
    {
        msg_type = MsgType::HOST_RESP;
        error_code = ErrorCode::NONE;
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(hosts);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(hosts, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        if ((size1 = Msg::deserialize(buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(hosts, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}