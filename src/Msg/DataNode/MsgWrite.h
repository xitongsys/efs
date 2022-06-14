#pragma once

#include <string>

#include "Msg/Msg.h"

namespace efs {

struct MsgWrite : Msg {
    int32_t fd;
    std::string data;

    MsgWrite()
    {
        msg_type = MsgType::WRITE;
        error_code = ErrorCode::NONE;

        fd = 0;
    }
};

struct MsgWriteResp : Msg {
    int32_t fd;
    int32_t write_size;

    MsgWriteResp()
    {
        msg_type = MsgType::WRITE_RESP;
        error_code = ErrorCode::NONE;

        fd = 0;
        write_size = 0;
    }
};
}