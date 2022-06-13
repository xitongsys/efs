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
    Permission permission(const std::string& path, int64_t uid, int64_t gid);

    std::shared_ptr<MsgLoginResp> login(std::shared_ptr<MsgLogin> p_msg_login);
    std::shared_ptr<MsgLsResp> ls(std::shared_ptr<MsgLs> p_msg_ls);
};
}