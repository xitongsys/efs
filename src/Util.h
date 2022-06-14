#include <chrono>
#include <stdint.h>

namespace efs {
namespace util {

    inline int64_t now()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    template <typename T>
    inline bool isPower2(const T& value)
    {
        return (x != 0) && !(x & (x - 1));
    }
}
}