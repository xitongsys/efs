#include <boost/algorithm/string.hpp>
#include <filesystem>

namespace efs {
    
inline std::string formatPath(const std::string& path)
{
    std::string path1 = path;
    boost::algorithm::trim(path1);
    while (path1.size() > 1 && (*path1.rbegin()) == '/') {
        path1.pop_back();
    }
    return std::move(path1);
}

inline bool exists(const std::string& path)
{
    return std::filesystem::exists(path);
}

}