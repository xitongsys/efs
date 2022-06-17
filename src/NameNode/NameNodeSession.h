#pragma once

#include <memory>
#include <unordered_set>

#include "NameNodeConfig.h"
#include "SessionBase.h"
#include "UserDesc.h"

namespace efs {

class NameNodeSession : public SessionBase {
public:
    NameNodeConfig& config;
    std::shared_ptr<Msg> p_in_msg, p_out_msg;

public:
    NameNodeSession(int32_t buffer_size,
        boost::asio::ip::tcp::socket socket,
        NameNodeConfig& config);

    ~NameNodeSession();

    ErrorCode readMsgHandler();
    ErrorCode writeMsgHandler();

    void accout();
    void host();
};
}