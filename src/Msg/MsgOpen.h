#include <string>

#include "Msg/Msg.h"

namespace efs {
enum OpenMod {
    R = 0x1,
    W = 0x2,
    RW = 0x3,
    A = 0x4,
};

struct MsgOpen : Msg {
    OpenMod mod;
    std::string file_path;

    MsgOpen()
    {
        msg_type = OPEN;
        mod = R;
        file_path = "";
    }
};

struct MsgOpenResp : Msg {
    uint64_t fd;
    int8_t ec;

    MsgOpenResp()
    {
        fd = -1;
    }
};
}