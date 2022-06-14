#include <string>

namespace efs {
struct UserDesc {
    std::string account;
    std::string password;
    int32_t uid;
    int32_t gid;
};
}
