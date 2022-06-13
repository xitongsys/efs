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

    int32_t size()
    {
        return Msg::size() + serialize::size(user) + serialize::size(password);
    }

    int32_t serialize(char* buf, int32_t buf_size)
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

    int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int size = 0, size1 = 0;
        size1 = Msg::deserialize(buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        size1 = serialize::deserialize(user, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        size1 = serialize::deserialize(password, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};

struct MsgLoginResp : Msg {

    int64_t uid, gid;

    MsgLoginResp()
    {
        msg_type = MsgType::LOGIN_RESP;
        uid = -1;
        gid = -1;
    }

    int32_t size()
    {
        return Msg::size() + sizeof(uid) + sizeof(gid);
    }

    int32_t serialize(char* buf, int32_t buf_size)
    {
        if (this->size() > buf_size) {
            return -1;
        }
        int32_t size = 0;
        size += Msg::serialize(buf + size, buf_size - size);
        size += serialize::serialize(uid, buf + size, buf_size - size);
        size += serialize::serialize(gid, buf + size, buf_size - size);
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

        size1 = serialize::deserialize(uid, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        size1 = serialize::deserialize(gid, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}