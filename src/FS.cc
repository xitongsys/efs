#include <array>
#include <boost/algorithm/string.hpp>
#include <filesystem>

#include "FS.h"

namespace efs {
namespace fs {

    constexpr std::array<uint8_t, (1 << 8)> CS {
        []() constexpr {
            std::array<uint8_t, (1 << 8)> res {};
    constexpr char cs[] = "0123456789abcdefghijklmnopqrstuvwxyz_-.() @!~`#$%^&+={}[]|;,";
    const char* p = cs;
    while (*p) {
        res[*p] = 1;
        p++;
    }
    return res;
}
()
};
}
}