#include <string>

namespace efs {

enum FileType {
    FILE,
    DIR
};

enum Permission {
    R = 0x1,
    W = 0x2,
    X = 0x4,
};

struct FileDesc {
    std::string path;
    FileType type;
    uint64_t size;

    uint64_t uid;
    uint64_t gid;
    uint16_t mod;
};
}