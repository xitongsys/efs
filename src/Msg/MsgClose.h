#include <string>

#include "Msg/Msg.h"

namespace efs {
struct MsgClose : Msg {
    int64_t fd;

    MsgClose()
    {
        msg_type = MsgType::CLOSE;
        error_code = ErrorCode::NONE;

        fd = -1;
    }
};

struct MsgCloseResp : Msg {

    MsgCloseResp()
    {
        msg_type = MsgType::CLOSE_RESP;
        error_code = ErrorCode::NONE;
    }
};
}