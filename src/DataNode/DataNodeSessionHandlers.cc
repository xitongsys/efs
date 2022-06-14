#include "DataNode/DataNodeSession.h"
#include "Msg/MsgChmod.h"
#include "Msg/MsgChown.h"
#include "Msg/MsgClose.h"
#include "Msg/MsgLogin.h"
#include "Msg/MsgLs.h"
#include "Msg/MsgMkdir.h"
#include "Msg/MsgOpen.h"
#include "Msg/MsgRead.h"
#include "Msg/MsgRm.h"
#include "Msg/MsgWrite.h"

namespace efs {
void DataNodeSession::login()
{
    std::shared_ptr<MsgLogin> p_in_msg = std::static_pointer_cast<MsgLogin>(this->p_in_msg);
    std::shared_ptr<MsgLoginResp> p_out_msg = std::make_shared<MsgLoginResp>();
    ErrorCode ec = ErrorCode::NONE;
    p_out_msg->error_code = p_executor->login(p_in_msg->user, p_in_msg->password, this->udesc);

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::ls()
{
    std::shared_ptr<MsgLs> p_in_msg = std::static_pointer_cast<MsgLs>(this->p_in_msg);
    std::shared_ptr<MsgLsResp> p_out_msg = std::make_shared<MsgLsResp>();

    Permission perm = Permission::NONE;
    ErrorCode ec;
    if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
        p_out_msg->error_code = ec;

    } else if (perm & Permission::R) {
        ec = p_executor->ls(p_in_msg->path, p_out_msg->files);
        p_out_msg->error_code = ec;
    }

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::rm()
{
    std::shared_ptr<MsgRm> p_in_msg = std::static_pointer_cast<MsgRm>(this->p_in_msg);
    std::shared_ptr<MsgRmResp> p_out_msg = std::make_shared<MsgRmResp>();

    Permission perm = Permission::NONE;
    ErrorCode ec;
    if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
        p_out_msg->error_code = ec;

    } else if ((perm & Permission::W)) {
        ec = p_executor->rm(p_in_msg->path);
        p_out_msg->error_code = ec;

    } else {
        p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
    }

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::chown()
{
    std::shared_ptr<MsgChown> p_in_msg = std::static_pointer_cast<MsgChown>(this->p_in_msg);
    std::shared_ptr<MsgChownResp> p_out_msg = std::make_shared<MsgChownResp>();

    Permission perm = Permission::NONE;
    ErrorCode ec;
    if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
        p_out_msg->error_code = ec;

    } else if ((perm & Permission::W)) {
        ec = p_executor->rm(p_in_msg->path);
        p_out_msg->error_code = ec;

    } else {
        p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
    }

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::chmod()
{
    std::shared_ptr<MsgChmod> p_in_msg = std::static_pointer_cast<MsgChmod>(this->p_in_msg);
    std::shared_ptr<MsgChmodResp> p_out_msg = std::make_shared<MsgChmodResp>();

    Permission perm = Permission::NONE;
    ErrorCode ec;
    if ((ec = p_executor->permission(p_in_msg->path, this->udesc.uid, this->udesc.gid, perm))) {
        p_out_msg->error_code = ec;

    } else if ((perm & Permission::W)) {
        ec = p_executor->chmod(p_in_msg->path, p_in_msg->mod);
        p_out_msg->error_code = ec;

    } else {
        p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
    }

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::mkdir()
{
    ErrorCode ec = ErrorCode::NONE;
    std::shared_ptr<MsgMkdir> p_in_msg = std::static_pointer_cast<MsgMkdir>(this->p_in_msg);
    std::shared_ptr<MsgMkdirResp> p_out_msg = std::make_shared<MsgMkdirResp>();

    do {
        std::string parent_path;
        if (ec = p_executor->parentPath(p_in_msg->path, parent_path)) {
            p_out_msg->error_code = ec;
            break;
        }

        Permission perm = Permission::NONE;
        if ((ec = p_executor->permission(parent_path, this->udesc.uid, this->udesc.gid, perm))) {
            p_out_msg->error_code = ec;

        } else if ((perm & Permission::W)) {
            ec = p_executor->mkdir(p_in_msg->path, this->udesc.uid, this->udesc.gid);
            p_out_msg->error_code = ec;

        } else {
            p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
        }

    } while (0);

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::open()
{
    ErrorCode ec = ErrorCode::NONE;
    std::shared_ptr<MsgOpen> p_in_msg = std::static_pointer_cast<MsgOpen>(this->p_in_msg);
    std::shared_ptr<MsgOpenResp> p_out_msg = std::make_shared<MsgOpenResp>();
    const std::string& path = p_in_msg->path;
    const std::string& open_mod = p_in_msg->open_mod;

    do {
        if (this->open_files.size() >= DataNodeSession::MAX_OPEN_FILE_NUM) {
            p_out_msg->error_code = E_FILE_TOO_MANY;
            break;
        }

        FileDesc fdesc;
        if ((ec = p_executor->getFileDesc(path, fdesc))) {
            std::string parent_path;
            if (ec = p_executor->parentPath(path, parent_path)) {
                p_out_msg->error_code = ec;
                break;
            }

            FileDesc parent_fdesc;
            if ((ec = p_executor->getFileDesc(parent_path, parent_fdesc))) {
                p_out_msg->error_code = ec;
                break;
            }

            Permission perm = parent_fdesc.perm(this->udesc.uid, this->udesc.gid);
            if (!(perm & Permission::W)) {
                p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
                break;
            }

            if (open_mod.find('+') == std::string::npos) {
                p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
                break;
            }

            OpenFileHandler fh;
            if ((ec = p_executor->open(path, open_mod, this->udesc.uid, this->udesc.gid, fh))) {
                p_out_msg->error_code = ec;
                break;
            }

            this->open_files[fh.fd] = fh;
            p_out_msg->fd = fh.fd;
        }

    } while (0);

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::close()
{
    ErrorCode ec = ErrorCode::NONE;
    std::shared_ptr<MsgClose> p_in_msg = std::static_pointer_cast<MsgClose>(this->p_in_msg);
    std::shared_ptr<MsgCloseResp> p_out_msg = std::make_shared<MsgCloseResp>();

    int32_t fd = p_in_msg->fd;
    if (this->open_files.count(fd)) {
        p_executor->close(this->open_files[fd]);

        OpenFileHandler& fh = this->open_files[fd];
        FileDesc fdesc;
        if (!(p_executor->getFileDescFromDisk(fh.fdesc.path, fdesc))) {
            // actually only modified_time and fsize can be get from the host filesystem

            fh.fdesc.modified_time = fdesc.modified_time;
            fh.fdesc.fsize = fdesc.fsize;

            // TODO:: if error, what should do ?
            p_executor->setFileDesc(fh.fdesc.path, fh.fdesc);
        }

        this->open_files.erase(fd);
    }

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::read()
{
    ErrorCode ec = ErrorCode::NONE;
    std::shared_ptr<MsgClose> p_in_msg = std::static_pointer_cast<MsgClose>(this->p_in_msg);
    std::shared_ptr<MsgCloseResp> p_out_msg = std::make_shared<MsgCloseResp>();

    int32_t fd = p_in_msg->fd;
    if (this->open_files.count(fd)) {
        p_executor->close(this->open_files[fd]);
        this->open_files.erase(fd);
    }

    this->p_out_msg = p_out_msg;
}

void DataNodeSession::write()
{
    ErrorCode ec = ErrorCode::NONE;
    std::shared_ptr<MsgClose> p_in_msg = std::static_pointer_cast<MsgClose>(this->p_in_msg);
    std::shared_ptr<MsgCloseResp> p_out_msg = std::make_shared<MsgCloseResp>();

    int32_t fd = p_in_msg->fd;
    if (this->open_files.count(fd)) {
        p_executor->close(this->open_files[fd]);
        this->open_files.erase(fd);
    }

    this->p_out_msg = p_out_msg;
}

}
