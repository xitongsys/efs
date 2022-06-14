#include "DataNode/DataNodeSession.h"
#include "Msg/MsgClose.h"
#include "Msg/MsgLogin.h"
#include "Msg/MsgLs.h"
#include "Msg/MsgOpen.h"

namespace efs {
void DataNodeSession::ls()
{
    std::shared_ptr<MsgLs> p_in_msg = std::static_pointer_cast<MsgLs>(this->p_in_msg);
    std::shared_ptr<MsgLsResp> p_out_msg = std::make_shared<MsgLsResp>();

    Permission perm = Permission::NONE;
    ErrorCode ec;
    if ((ec = p_executor->permission(p_in_msg->path, uid, gid, perm))) {
        p_out_msg->error_code = ec;

    } else if (perm & Permission::R) {
        ec = p_executor->ls(p_in_msg->path, p_out_msg->files);
        p_out_msg->error_code = ec;
    }

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
        if ((ec = p_executor->fileDesc(path, fdesc))) {
            std::string parent_path = p_executor->parentPath(path);
            FileDesc parent_fdesc;
            if ((ec = p_executor->fileDesc(parent_path, parent_fdesc))) {
                p_out_msg->error_code = ec;
                break;
            }

            Permission perm = parent_fdesc.perm(this->uid, this->gid);
            if (!(perm & Permission::W)) {
                p_out_msg->error_code = ErrorCode::E_FILE_PERMISSION;
                break;
            }

            if (open_mod.find('+') == std::string::npos) {
                p_out_msg->error_code = ErrorCode::E_FILE_NOT_FOUND;
                break;
            }

            OpenFileHandler fh;
            if ((ec = p_executor->open(path, open_mod, this->uid, this->gid, fh))) {
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
        p_executor->close(&(this->open_files[fd]));
        this->open_files.erase(fd);
    }

    this->p_out_msg = p_out_msg;
}

}
