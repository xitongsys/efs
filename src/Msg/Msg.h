#include <string>

namespace efs {
enum MsgType {
    OPEN,
    CLOSE,
    WRITE,
    READ,
    LS,
    MKDIR,
    RM,
    CHMOD,
    CHOWN,
};

struct Msg {
    MsgType msg_type;
};
}