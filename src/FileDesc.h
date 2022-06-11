#include <string>

namespace efs {
    enum FileType {
        FILE,
        DIR
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