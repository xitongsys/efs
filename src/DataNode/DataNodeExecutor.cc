#include <cstdio>
#include <filesystem>

#include "DataNodeExecutor.h"
#include "FS.h"

namespace efs {
DataNodeExecutor::DataNodeExecutor(const DataNodeConfig& config)
    : db(config.log_path)
{
    this->config = config;
}

DataNodeExecutor::~DataNodeExecutor()
{
}

std::string DataNodeExecutor::absolutePath(const std::string& path)
{
    auto absolute_path = std::filesystem::path(config.root_path) / path;
    return absolute_path.string();
}

std::string DataNodeExecutor::relativePath(const std::string& path)
{
    int n = path.size();
    return path.substr(config.root_path.size(), n - config.root_path.size());
}

ErrorCode DataNodeExecutor::fileDesc(const std::string& path, FileDesc& fdesc)
{
    std::string value;
    if (db.get(path, value)) {
        return E_DB_GET;
    }

    if (fdesc.deserialize(value.c_str(), value.size()) < 0) {
        return E_DESERIALIZE;
    }

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::permission(const std::string& path, int64_t uid, int64_t gid, Permission& perm)
{
    FileDesc fdesc;
    ErrorCode ec;

    if ((ec = fileDesc(path, fdesc))) {
        return ec;
    }

    if (fdesc.uid == uid) {
        perm = Permission((fdesc.mod >> 6) & 0b111);

    } else if (fdesc.gid == gid) {
        perm = Permission((fdesc.mod >> 3) & 0b111);

    } else {
        perm = Permission(fdesc.mod & 0b111);
    }

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::login(const std::string& user, const std::string& password)
{
    if (users.count(user) == 0) {
        return ErrorCode::E_LOGIN_USER_NOT_FOUND;

    } else if (users[user].password != password) {
        return ErrorCode::E_LOGIN_WRONG_PASSWORD;

    } else {
        return ErrorCode::NONE;
    }
}

ErrorCode DataNodeExecutor::ls(const std::string& path, std::vector<FileDesc>& fs)
{
    std::string absolute_path = absolutePath(path);
    for (const auto& entry : std::filesystem::directory_iterator(absolute_path)) {
        std::string relative_path = relativePath(entry.path());
        FileDesc fdesc;
        ErrorCode ec;
        if ((ec = fileDesc(relative_path, fdesc))) {
            return ec;
        }
        fs.push_back(fdesc);
    }

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::open(const std::string& path, const std::string& mod, FILE** fp)
{
    if (((*fp) = fopen(path.c_str(), mod.c_str())) == NULL) {
        return ErrorCode::E_FILE_OPEN;
    }
    return ErrorCode::NONE;
}

}