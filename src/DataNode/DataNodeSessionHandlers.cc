#include "DataNode/DataNodeSession.h"
#include "Msg/MsgLs.h"
#include "Msg/MsgOpen.h"

namespace efs {
void DataNodeSession::lsHandler()
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

void DataNodeSession::openHandler()
{
    std::shared_ptr<MsgOpen> p_in_msg = std::static_pointer_cast<MsgOpen>(this->p_in_msg);
    std::shared_ptr<MsgOpenResp> p_out_msg = std::make_shared<MsgOpenResp>();

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

}
