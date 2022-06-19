#pragma once

#include <memory>
#include <unordered_map>

#include "DBBase.h"
#include "DataNode/DataNodeConfig.h"
#include "DataNode/OpenFileHandler.h"
#include "FileDesc.h"
#include "GroupDesc.h"
#include "HostDesc.h"
#include "UserDesc.h"

namespace efs {

class DataNodeExecutor {
public:
    DBBase db;
    HostDesc hdesc;
    std::unordered_map<std::string, UserDesc> users;
    std::unordered_map<std::string, GroupDesc> groups;

public:
    DataNodeExecutor(const DataNodeConfig& config);
    ~DataNodeExecutor();
    DataNodeConfig config;

    ErrorCode init();
    ErrorCode updateAccount();

    ErrorCode getUser(const std::string& name, UserDesc& udesc);
    ErrorCode getGroup(const std::string& name, GroupDesc& gdesc);

    ErrorCode absolutePath(const std::string& path, std::string& absolute_path);
    ErrorCode relativePath(const std::string& path, std::string& relative_path);
    ErrorCode parentPath(const std::string& path, std::string& parent_path);

    ErrorCode permission(const std::string& path, int16_t uid, int16_t gid, Permission& perm);

    ErrorCode getFileDesc(const std::string& path, FileDesc& fdesc);
    ErrorCode setFileDesc(const std::string& path, const FileDesc& fdesc);
    ErrorCode getFileDescFromDisk(const std::string& path, FileDesc& fdesc);

    ErrorCode login(const std::string& user, const std::string& password, UserDesc& udesc);
    ErrorCode ls(const std::string& path, std::vector<FileDesc>& fs);
    ErrorCode rm(const std::string& path);
    ErrorCode chown(const std::string& path, int16_t uid, int16_t gid);
    ErrorCode chmod(const std::string& path, uint16_t mod);
    ErrorCode perm(const std::string& path, uint16_t id, PermType perm_type, Permission p);
    ErrorCode mkdir(const std::string& path, int16_t uid, int16_t gid, const FileDesc& parent_desc);

    ErrorCode open(const std::string& path, const std::string& open_mod, int16_t uid, int16_t gid, const FileDesc& parent_desc, OpenFileHandler& fh);
    ErrorCode close(OpenFileHandler& fh);
    ErrorCode write(OpenFileHandler& fh, const char* buf, const int32_t& buf_size, int32_t& write_size);
    ErrorCode read(OpenFileHandler& fh, char* buf, const int32_t& buf_size, int32_t& read_size);
};
}