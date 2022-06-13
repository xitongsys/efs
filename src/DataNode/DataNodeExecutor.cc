#include <filesystem>

#include "DataNodeExecutor.h"
#include "FS.h"

namespace efs {
DataNodeExecutor::DataNodeExecutor(const DataNodeConfig& config)
{
    this->config = config;
    auto db_path = std::filesystem::path(config.log_path) / "efs.db";
}

DataNodeExecutor::~DataNodeExecutor()
{
}

std::string DataNodeExecutor::absolutePath(const std::string& path)
{
    auto absolute_path = std::filesystem::path(config.root_path) / path;
    return absolute_path.string();
}

Permission DataNodeExecutor::permission(const std::string& path, int64_t uid, int64_t gid)
{
    std::string path_f = formatPath(path);
    std::string absolute_path = absolutePath(path_f);
}

std::shared_ptr<MsgLoginResp> DataNodeExecutor::login(std::shared_ptr<MsgLogin> p_msg_login)
{
    std::shared_ptr<MsgLoginResp> p_msg_login_resp = std::make_shared<MsgLoginResp>();
    const std::string& user = p_msg_login->user;
    if (users.count(user) == 0) {
        p_msg_login_resp->error_code = ErrorCode::E_LOGIN_ACCOUNT_NOT_FOUND;

    } else if (users[user].password != p_msg_login->password) {
        p_msg_login_resp->error_code = ErrorCode::E_LOGIN_WRONG_PASSWORD;

    } else {
        p_msg_login_resp->uid = users[user].uid;
        p_msg_login_resp->gid = users[user].gid;
    }

    return p_msg_login_resp;
}

std::shared_ptr<MsgLsResp> DataNodeExecutor::ls(std::shared_ptr<MsgLs> p_msg_ls)
{
}

}