#include <memory>
#include <unordered_map>

#include "DBBase.h"
#include "DataNode/DataNodeConfig.h"
#include "FileDesc.h"
#include "Msg/MsgLogin.h"
#include "Msg/MsgLs.h"
#include "UserDesc.h"

namespace efs {
class DataNodeExecutor {
public:
    DBBase db;
    std::unordered_map<std::string, UserDesc> users;

public:
    DataNodeExecutor(const DataNodeConfig& config);
    ~DataNodeExecutor();
    DataNodeConfig config;

    std::string absolutePath(const std::string& path);
    std::string relativePath(const std::string& path);

    ErrorCode permission(const std::string& path, int64_t uid, int64_t gid, Permission& perm);
    ErrorCode fileDesc(const std::string& path, FileDesc& fdesc);

    ErrorCode login(const std::string& user, const std::string& password);
    ErrorCode ls(const std::string& path, std::vector<FileDesc>& fs);
};
}