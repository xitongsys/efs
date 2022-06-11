#include <string>
#include <vector>

#include "FileDesc.h"
#include "Msg/Msg.h"

namespace efs {
    struct MsgLs : Msg {
        std::string path;
    };

    struct MsgLsResp : Msg {
        std::vector<FileDesc> files;
        int8_t ec;

        MsgLsResp() {
            ec = 0;
        }
    };
}