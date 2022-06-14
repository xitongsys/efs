#include <string>
#include <vector>

#include "UserDesc.h"

namespace efs {
class NameNodeConfig {
public:
    std::string name;
    std::string ip;
    int16_t port;

    std::vector<UserDesc> users;

    std::string log_path;
};
}