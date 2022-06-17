#include <algorithm>
#include <cmath>
#include <iostream>

#include "Msg/NameNode/MsgAccount.h"
#include "Msg/NameNode/MsgHost.h"
#include "NameNode/NameNodeSession.h"

namespace efs {
void NameNodeSession::account()
{
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
