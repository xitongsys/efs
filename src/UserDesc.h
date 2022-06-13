#include <string>

namespace efs {
struct UserDesc {
    std::string account;
    std::string password;
    int64_t uid;
    int64_t gid;
};
}
