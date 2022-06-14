#include <string>

#include "Msg/Msg.h"

namespace efs {

struct MsgRead : Msg {
    int32_t fd;
    int32_t read_size;

    MsgRead()
    {
        msg_type = MsgType::READ;
        error_code = ErrorCode::NONE;

        fd = 0;
        read_size = 0;
    }
};

struct MsgReadResp : Msg {
    int32_t fd;
    std::string data;

    MsgReadResp()
    {
        msg_type = MsgType::READ_RESP;
        error_code = ErrorCode::NONE;

        fd = 0;
    }
};
}