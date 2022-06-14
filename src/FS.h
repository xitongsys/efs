#include <array>
#include <boost/algorithm/string.hpp>
#include <filesystem>

namespace efs {
namespace fs {
    constexpr std::array<uint8_t, (1 << 8)> CS;

    inline std::string formatPath(const std::string& path)
    {
        int n = path.size();
        std::string res;
        for (int i = 0; i < n; i++) {
            char c = path[i];
            if (c == '\\') {
                c = '/';
            }

            if (CS[c]) {
                res.push_back(c);
            } else if (c == '/') {
                if (res.size() == 0 || (*res.rbegin()) != '/') {
                    res.push_back(c);
                }
            }
        }
        if (res.size() == 0) {
            return "/";
        }

        if (res.size() > 1 && *(res.rbegin()) == '/') {
            res.pop_back();
        }

        return std::move(res);
    }

    inline bool exists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }

}
}