#include <string>

namespace efs {
struct UserDesc {
    std::string user;
    std::string password;
    int32_t uid;
    int32_t gid;
};
}
