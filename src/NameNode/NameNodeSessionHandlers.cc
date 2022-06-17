#include <algorithm>
#include <cmath>
#include <iostream>

#include "Error.h"
#include "Msg/NameNode/MsgAccount.h"
#include "Msg/NameNode/MsgHost.h"
#include "NameNode/NameNodeSession.h"

namespace efs {
void NameNodeSession::account()
{
    std::shared_ptr<MsgAccount> p_in_msg = std::static_pointer_cast<MsgAccount>(this->p_in_msg);
    std::shared_ptr<MsgAccountResp> p_out_msg = std::make_shared<MsgAccountResp>();

    do {
        if (namenode.config.tokens.count(p_in_msg->hdesc.token)) {
            p_out_msg->error_code = ErrorCode::E_TOKEN_ERROR;
            break;
        }

        p_out_msg->users = namenode.config.users;
        p_out_msg->groups = namenode.config.groups;

    } while (0);

    this->p_out_msg = p_out_msg;
}

void NameNodeSession::host()
{
    std::shared_ptr<MsgHost> p_in_msg = std::static_pointer_cast<MsgHost>(this->p_in_msg);
    std::shared_ptr<MsgHostResp> p_out_msg = std::make_shared<MsgHostResp>();

    for (auto host : namenode.hosts) {
        host.token = "";
        p_out_msg->hosts.push_back(host);
    }

    this->p_out_msg = p_out_msg;
}

}
