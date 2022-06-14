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

    inline bool isFile(const std::string& path)
    {
        return std::filesystem::is_regular_file(path);
    }

    inline bool isDirectory(const std::string& path)
    {
        return std::filesystem::is_directory(path);
    }

    inline int64_t fileSize(const std::string& path)
    {
        return std::filesystem::file_size(path);
    }

    inline int64_t modifiedTime(const std::string& path)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::filesystem::last_write_time(path).time_since_epoch())
            .count();
    }

}
}