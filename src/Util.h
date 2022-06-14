#pragma once

#include <chrono>
#include <stdint.h>
#include <string>
#include <vector>

namespace efs {
namespace util {

    inline int64_t now()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    template <typename T>
    inline bool isPower2(const T& x)
    {
        return (x != 0) && !(x & (x - 1));
    }

    inline std::vector<std::string> split(const std::string& s, char d)
    {
        std::vector<std::string> res;
        int ns = s.size();
        int i = 0, j = 0;
        while (i < ns) {
            j = i;
            while (j < ns && s[j] != d) {
                j++;
            }
            res.push_back(s.substr(i, j - i));
            i = j;
        }
        return res;
    }
}
}