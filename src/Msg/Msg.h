#include <string>

#include "Serialize.h"

namespace efs {
enum MsgType : uint8_t {
    NONE,
    LOGIN,
    LOGIN_RESP,

    OPEN,
    OPEN_RESP,

    CLOSE,
    CLOSE_RESP,

    WRITE,
    WRITE_RESP,

    READ,
    READ_RESP,

    LS,
    LS_RESP,

    MKDIR,
    MKDIR_RESP,

    RM,
    RM_RESP,

    CHMOD,
    CHMOD_RESP,

    CHOWN,
    CHOWN_RESP,
};

struct Msg {
    MsgType msg_type;

    int8_t error_code;

    Msg()
    {
        msg_type = MsgType::NONE;
        error_code = 0;
    }

    int32_t size()
    {
        return sizeof(MsgType) + sizeof(int8_t);
    }

    int32_t serialize(char* buf, int32_t buf_size)
    {
        if (this->size() > buf_size) {
            return -1;
        }

        int32_t size = 0;
        size += serialize::serialize(msg_type, buf + size, buf_size - size);
        size += serialize::serialize(error_code, buf + size, buf_size - size);
        return size;
    }

    int32_t deserialize(const char* buf, int32_t buf_size)
    {
        int32_t size = 0, size1 = 0;
        size1 = serialize::deserialize(msg_type, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        size1 = serialize::deserialize(error_code, buf + size, buf_size - size);
        if (size1 < 0) {
            return -1;
        }
        size += size1;

        return size;
    }
};
}