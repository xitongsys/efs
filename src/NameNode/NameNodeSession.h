#pragma once

#include <memory>
#include <unordered_set>

#include "NameNode/NameNode.h"
#include "NameNode/NameNodeConfig.h"
#include "SessionBase.h"
#include "UserDesc.h"

namespace efs {

class NameNode;

class NameNodeSession : public SessionBase {
public:
    NameNode& namenode;
    std::shared_ptr<Msg> p_in_msg, p_out_msg;

public:
    NameNodeSession(int32_t buffer_size,
        boost::asio::ip::tcp::socket socket,
        NameNode& namenode);

    ~NameNodeSession();

    ErrorCode readMsgHandler();
    ErrorCode writeMsgHandler();

    void account();
    void host();
};
}