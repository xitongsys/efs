#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "DataNodeExecutor.h"
#include "FS.h"
#include "Util.h"

namespace efs {
DataNodeExecutor::DataNodeExecutor(const DataNodeConfig& config)
    : db(config.log_path)
{
    this->config = config;

    for (const std::string& csv : config.users) {
        UserDesc udesc(csv);
        this->users[udesc.user] = udesc;
    }

    init();
}

DataNodeExecutor::~DataNodeExecutor()
{
}

ErrorCode DataNodeExecutor::init()
{
    ErrorCode ec = ErrorCode::NONE;
    FileDesc fdesc;
    fdesc.path = "/";
    fdesc.mod = 0b1111001001;
    fdesc.uid = 1;
    fdesc.gid = 1;

    this->setFileDesc(fdesc.path, fdesc);

    for (auto it = users.begin(); it != users.end(); it++) {
        UserDesc& udesc = it->second;
        ec = this->mkdir(udesc.root_path, udesc.uid, udesc.gid);
    }

    return ec;
}

ErrorCode DataNodeExecutor::absolutePath(const std::string& path, std::string& absolute_path)
{
    absolute_path = fs::formatPath(config.root_path + "/" + path);
    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::relativePath(const std::string& path, std::string& relative_path)
{
    int n = config.root_path.size();

    relative_path = fs::formatPath(path);
    if (relative_path.size() < n) {
        return ErrorCode::E_FILE_PATH;
    }

    if (relative_path.substr(0, n) != config.root_path) {
        return ErrorCode::E_FILE_PATH;
    }
    relative_path = fs::formatPath(relative_path.substr(n, relative_path.size() - n));

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::parentPath(const std::string& path, std::string& parent_path)
{
    parent_path = fs::formatPath(path);
    while (parent_path.size() > 0 && (*parent_path.rbegin()) != '/') {
        parent_path.pop_back();
    }

    if (parent_path.size() > 1 && (*parent_path.rbegin()) == '/') {
        parent_path.pop_back();
    }

    if (parent_path.size() == 0) {
        return ErrorCode::E_FILE_PATH;
    }
    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::getFileDesc(const std::string& path, FileDesc& fdesc)
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

ErrorCode DataNodeExecutor::setFileDesc(const std::string& path, const FileDesc& fdesc)
{
    std::string value(fdesc.size(), 0);
    fdesc.serialize(value.data(), value.size());
    return db.put(path, value);
}

ErrorCode DataNodeExecutor::getFileDescFromDisk(const std::string& path, FileDesc& fdesc)
{
    ErrorCode ec = ErrorCode::NONE;
    std::string absolute_path;
    if (ec = absolutePath(path, absolute_path)) {
        return ec;
    }

    if (!fs::exists(absolute_path)) {
        return ErrorCode::E_FILE_NOT_FOUND;
    }

    fdesc.fsize = fs::fileSize(absolute_path);
    fdesc.modified_time = fs::modifiedTime(absolute_path);

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::permission(const std::string& path, int32_t uid, int32_t gid, Permission& perm)
{
    FileDesc fdesc;
    ErrorCode ec;

    if ((ec = getFileDesc(path, fdesc))) {
        return ec;
    }

    // for root user
    if (uid == 1) {
        perm = Permission(0b111);
        return ErrorCode::NONE;
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

ErrorCode DataNodeExecutor::login(const std::string& user, const std::string& password, UserDesc& udesc)
{
    if (users.count(user) == 0) {
        return ErrorCode::E_LOGIN_USER_NOT_FOUND;

    } else if (users[user].password != password) {
        return ErrorCode::E_LOGIN_WRONG_PASSWORD;

    } else {
        udesc = users[user];
        return ErrorCode::NONE;
    }
}

ErrorCode DataNodeExecutor::ls(const std::string& path, std::vector<FileDesc>& fs)
{
    ErrorCode ec = ErrorCode::NONE;
    std::string absolute_path;
    if (ec = absolutePath(path, absolute_path)) {
        return ec;
    }

    for (const auto& entry : std::filesystem::directory_iterator(absolute_path)) {
        std::string relative_path;
        if (ec = relativePath(entry.path(), relative_path)) {
            return ec;
        }
        FileDesc fdesc;
        if ((ec = getFileDesc(relative_path, fdesc))) {
        } else {
            fs.push_back(fdesc);
        }
    }

    return ec;
}

ErrorCode DataNodeExecutor::rm(const std::string& path)
{
    ErrorCode ec = ErrorCode::NONE;
    if ((ec = db.del(path))) {
        ec = ErrorCode::E_FILE_RM;
    }
    std::string absolute_path;
    if (ec = absolutePath(path, absolute_path)) {
        return ec;
    }

    if (!std::filesystem::remove_all(absolute_path.c_str())) {
        ec = ErrorCode::E_FILE_RM;
    }
    return ec;
}

ErrorCode DataNodeExecutor::chown(const std::string& path, int32_t uid, int32_t gid)
{
    ErrorCode ec = ErrorCode::NONE;
    FileDesc fdesc;
    if ((ec = getFileDesc(path, fdesc))) {
        return ec;
    }
    fdesc.uid = uid;
    fdesc.gid = gid;
    std::string value(fdesc.size(), 0);
    fdesc.serialize(value.data(), value.size());

    ec = db.put(path, value);
    return ec;
}

ErrorCode DataNodeExecutor::chmod(const std::string& path, uint16_t mod)
{
    ErrorCode ec = ErrorCode::NONE;
    FileDesc fdesc;
    if ((ec = getFileDesc(path, fdesc))) {
        return ec;
    }
    fdesc.mod = mod;
    std::string value(fdesc.size(), 0);
    fdesc.serialize(value.data(), value.size());

    ec = db.put(path, value);
    return ec;
}

ErrorCode DataNodeExecutor::mkdir(const std::string& path, int32_t uid, int32_t gid)
{
    ErrorCode ec = ErrorCode::NONE;
    FileDesc fdesc;
    fdesc.path = path;
    fdesc.uid = uid;
    fdesc.gid = gid;
    fdesc.mod = 0b1111000000;

    std::string value(fdesc.size(), 0);
    fdesc.serialize(value.data(), value.size());

    if ((ec = db.put(path, value))) {
        return ec;
    }

    std::string absolute_path;
    if (ec = absolutePath(path, absolute_path)) {
        return ec;
    }

    if (!std::filesystem::create_directory(absolute_path)) {
        ec = ErrorCode::E_FILE_MKDIR;
    }
    return ec;
}

ErrorCode DataNodeExecutor::open(const std::string& path, const std::string& mod, int32_t uid, int32_t gid, OpenFileHandler& fh)
{
    ErrorCode ec = ErrorCode::NONE;
    std::string parent_path;
    if (ec = parentPath(path, parent_path)) {
        return ec;
    }

    FileDesc fdesc;

    if ((ec = getFileDesc(parent_path, fdesc))) {
        return ec;
    }

    std::string absolute_path;

    if ((ec = absolutePath(path, absolute_path))) {
        return ec;
    }

    FILE* fp;
    if ((fp = fopen(absolute_path.c_str(), mod.c_str())) == NULL) {
        return ErrorCode::E_FILE_OPEN;
    }

    if (ec = getFileDesc(path, fdesc)) {
        fdesc.path = path;
        fdesc.mod = 0b0111000000;
        fdesc.uid = uid;
        fdesc.gid = gid;
        fdesc.create_time = util::now();
        fdesc.modified_time = util::now();
        fdesc.fsize = 0;

        if (ec = setFileDesc(path, fdesc)) {
            fclose(fp);
            fp = nullptr;
            return ec;
        }
    }

    fh.fd = fileno(fp);
    fh.fp = fp;
    fh.fdesc = fdesc;

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::close(OpenFileHandler& fh)
{
    fclose(fh.fp);
    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::write(OpenFileHandler& fh, const char* buf, const int32_t& buf_size, int32_t& write_size)
{
    write_size = std::fwrite(buf, sizeof(char), buf_size, fh.fp);
    if (write_size < buf_size) {
        return ErrorCode::E_FILE_WRITE;
    }
    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::read(OpenFileHandler& fh, char* buf, const int32_t& buf_size, int32_t& read_size)
{
    read_size = std::fread(buf, sizeof(char), buf_size, fh.fp);
    if (read_size < buf_size) {
        if (feof(fh.fp)) {
            return ErrorCode::E_FILE_EOF;
        }
        return ErrorCode::E_FILE_READ;
    }
    return ErrorCode::NONE;
}

}