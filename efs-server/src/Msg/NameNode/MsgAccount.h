#pragma once

#include <string>
#include <vector>

#include "GroupDesc.h"
#include "HostDesc.h"
#include "Msg/Msg.h"
#include "UserDesc.h"

namespace efs {

struct MsgAccount : Msg {
    HostDesc hdesc;

    MsgAccount()
    {
        msg_type = MsgType::ACCOUNT;
        error_code = ErrorCode::NONE;
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(hdesc);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(hdesc, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        if ((size1 = Msg::deserialize(buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(hdesc, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};

struct MsgAccountResp : Msg {
    std::vector<UserDesc> users;
    std::vector<GroupDesc> groups;

    MsgAccountResp()
    {
        msg_type = MsgType::ACCOUNT_RESP;
        error_code = ErrorCode::NONE;
    }

    inline int32_t size() const
    {
        return Msg::size() + serialize::size(users) + serialize::size(groups);
    }

    inline int32_t serialize(char* buf, int32_t buf_size) const
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(users, buf + size, buf_size - size);
        size += serialize::serialize(groups, buf + size, buf_size - size);
        return size;
    }

    inline int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        if ((size1 = Msg::deserialize(buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(users, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        if ((size1 = serialize::deserialize(groups, buf + size, buf_size - size)) < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}