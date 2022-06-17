#include <boost/asio.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "DataNodeExecutor.h"
#include "FS.h"
#include "HostDesc.h"
#include "Msg/NameNode/MsgAccount.h"
#include "Util.h"

namespace efs {
DataNodeExecutor::DataNodeExecutor(const DataNodeConfig& config)
    : db(config.log_path)
{
    this->config = config;
    hdesc.ip = config.ip;
    hdesc.port = config.port;
    hdesc.token = config.token;
    hdesc.paths = config.paths;

    init();
}

DataNodeExecutor::~DataNodeExecutor()
{
}

ErrorCode DataNodeExecutor::init()
{
    ErrorCode ec = ErrorCode::NONE;
    ec = updateAccount();
    for (auto& init_path : config.init_paths) {
        std::vector<std::string> vs = util::split(init_path, ',');
        if (vs.size() != 3) {
            throw ErrorCode::E_CONFIG_ERROR;
        }

        std::string path = vs[0];
        int16_t uid = std::atoi(vs[1].c_str());
        int16_t gid = std::atoi(vs[2].c_str());
        FileDesc fdesc;
        if ((ec = getFileDesc(path, fdesc))) {
            if ((ec = mkdir(path, uid, gid))) {
                throw ec;
            }
        }
    }
    return ec;
}

ErrorCode DataNodeExecutor::updateAccount()
{
    ErrorCode ec = ErrorCode::NONE;

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket sock(io_context);
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::connect(sock, resolver.resolve(config.name_node_ip, std::to_string(config.name_node_port)));

    constexpr int32_t BUF_SIZE = 4096;
    char buf[BUF_SIZE];
    MsgAccount msg_account;
    msg_account.hdesc = hdesc;
    int32_t msg_account_size = msg_account.size();
    msg_account.serialize(buf, BUF_SIZE);
    boost::asio::write(sock, boost::asio::buffer(buf, msg_account_size));

    MsgAccountResp msg_account_resp;
    int32_t read_size = 0;

    while (read_size < BUF_SIZE) {
        read_size += boost::asio::read(sock, boost::asio::buffer(buf + read_size, BUF_SIZE - read_size));
        if (msg_account_resp.deserialize(buf, read_size) == 0) {
            break;
        }
    }

    groups.clear();
    for (auto& group : msg_account_resp.groups) {
        groups[group.group] = group;
    }

    users.clear();
    for (auto& user : msg_account_resp.users) {
        users[user.user] = user;
    }

    return ec;
}

ErrorCode DataNodeExecutor::getUser(const std::string& name, UserDesc& udesc)
{
    if (this->users.count(name)) {
        udesc = users[name];
        return ErrorCode::NONE;
    }

    return ErrorCode::E_USER_NOT_FOUND;
}

ErrorCode DataNodeExecutor::getGroup(const std::string& name, GroupDesc& gdesc)
{
    if (this->groups.count(name)) {
        gdesc = groups[name];
        return ErrorCode::NONE;
    }

    return ErrorCode::E_GROUP_NOT_FOUND;
}

ErrorCode DataNodeExecutor::absolutePath(const std::string& path, std::string& absolute_path)
{
    absolute_path = fs::formatPath(config.root_path + "/" + path);
    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::relativePath(const std::string& path, std::string& relative_path)
{
    int32_t n = config.root_path.size();

    relative_path = fs::formatPath(path);
    if (int32_t(relative_path.size()) < n) {
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
    if ((ec = absolutePath(path, absolute_path))) {
        return ec;
    }

    if (!fs::exists(absolute_path)) {
        return ErrorCode::E_FILE_NOT_FOUND;
    }

    fdesc.fsize = fs::fileSize(absolute_path);
    fdesc.modified_time = fs::modifiedTime(absolute_path);

    return ErrorCode::NONE;
}

ErrorCode DataNodeExecutor::permission(const std::string& path, int16_t uid, int16_t gid, Permission& perm)
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
    if ((ec = absolutePath(path, absolute_path))) {
        return ec;
    }

    for (const auto& entry : std::filesystem::directory_iterator(absolute_path)) {
        std::string relative_path;
        if ((ec = relativePath(entry.path(), relative_path))) {
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
    if ((ec = absolutePath(path, absolute_path))) {
        return ec;
    }

    if (!std::filesystem::remove_all(absolute_path.c_str())) {
        ec = ErrorCode::E_FILE_RM;
    }
    return ec;
}

ErrorCode DataNodeExecutor::chown(const std::string& path, int16_t uid, int16_t gid)
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

ErrorCode DataNodeExecutor::perm(const std::string& path, int16_t id, PermType perm_type, Permission p)
{
    ErrorCode ec = ErrorCode::NONE;
    FileDesc fdesc;
    if ((ec = getFileDesc(path, fdesc))) {
        return ec;
    }

    if (perm_type == PermType::USER) {
        if (id == fdesc.uid) {
            fdesc.mod &= ~(0b111);
            fdesc.mod |= (p & 0b111) << perm_type;

        } else {
            int32_t i = 0;
            int32_t n = int32_t(fdesc.user_perms.size());
            for (i = 0; i < n; i++) {
                if (int16_t(fdesc.user_perms[i] >> 16) == id) {
                    break;
                }
            }
            if (i >= n) {
                fdesc.user_perms.push_back(Permission::EMPTY);
            }
            fdesc.user_perms[i] &= 0b111;
            fdesc.user_perms[i] |= p & 0b111;
        }

    } else if (perm_type == PermType::GROUP) {
        if (id == fdesc.gid) {
            fdesc.mod &= ~(0b111000);
            fdesc.mod |= (p & 0b111) << perm_type;

        } else {
            int32_t i = 0;
            int32_t n = int32_t(fdesc.group_perms.size());
            for (i = 0; i < n; i++) {
                if (int16_t(fdesc.group_perms[i] >> 16) == id) {
                    break;
                }
            }
            if (i >= n) {
                fdesc.group_perms.push_back(Permission::EMPTY);
            }
            fdesc.group_perms[i] &= 0b111;
            fdesc.group_perms[i] |= p & 0b111;
        }

    } else { // PermType::OTHER
        fdesc.mod &= ~(0b111000000);
        fdesc.mod |= (p & 0b111) << perm_type;
    }

    return ec;
}

ErrorCode DataNodeExecutor::mkdir(const std::string& path, int16_t uid, int16_t gid)
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
    if ((ec = absolutePath(path, absolute_path))) {
        return ec;
    }

    if (!std::filesystem::create_directory(absolute_path)) {
        ec = ErrorCode::E_FILE_MKDIR;
    }
    return ec;
}

ErrorCode DataNodeExecutor::open(const std::string& path, const std::string& mod, int16_t uid, int16_t gid, OpenFileHandler& fh)
{
    ErrorCode ec = ErrorCode::NONE;
    std::string parent_path;
    if ((ec = parentPath(path, parent_path))) {
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

    if ((ec = getFileDesc(path, fdesc))) {
        fdesc.path = path;
        fdesc.mod = 0b0111000000;
        fdesc.uid = uid;
        fdesc.gid = gid;
        fdesc.create_time = util::now();
        fdesc.modified_time = util::now();
        fdesc.fsize = 0;

        if ((ec = setFileDesc(path, fdesc))) {
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