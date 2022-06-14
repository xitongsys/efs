#include <string>

#include "Msg/Msg.h"

namespace efs {

struct MsgOpen : Msg {
    std::string path;
    std::string open_mod;

    MsgOpen()
    {
        msg_type = MsgType::OPEN;
        error_code = ErrorCode::NONE;

        path = "";
        open_mod = "";
    }
};

struct MsgOpenResp : Msg {
    int32_t fd;

    MsgOpenResp()
    {
        msg_type = MsgType::OPEN_RESP;
        error_code = ErrorCode::NONE;

        fd = -1;
    }
};
}