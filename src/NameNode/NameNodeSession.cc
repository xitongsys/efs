#include <iostream>

#include "NameNodeSession.h"

namespace efs {
NameNodeSession::NameNodeSession(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket,
    NameNodeConfig& config)
    : SessionBase(buffer_size,
        std::move(socket),
        std::bind(&NameNodeSession::readMsgHandler, this),
        std::bind(&NameNodeSession::writeMsgHandler, this))
    , config(config)
{
    p_in_msg = nullptr;
    p_out_msg = nullptr;
}

NameNodeSession::~NameNodeSession()
{
}

ErrorCode NameNodeSession::readMsgHandler()
{
    int32_t size = p_in_buffer->read_size();
    if (size == 0 || p_in_msg != nullptr) {
        return ErrorCode::NONE;
    }

#define RECV_IN_MSG(FUNC)                      \
    {                                          \
        int n = p_msg->deserialize(buf, size); \
        if (n > 0) {                           \
            p_in_msg = p_msg;                  \
            p_in_buffer->read_consume(n);      \
            FUNC();                            \
        }                                      \
    }

    if (p_in_msg == nullptr) {
        const char* buf = p_in_buffer->read_raw_buffer();
        switch (buf[0]) {
        case MsgType::ACCOUNT: {
            std::shared_ptr<MsgAccount> p_msg = std::make_shared<MsgAccount>();
            RECV_IN_MSG(account);
            break;
        }
        case MsgType::HOST: {
            std::shared_ptr<MsgHost> p_msg = std::make_shared<MsgHost>();
            RECV_IN_MSG(host);
            break;
        }
        default:
            return ErrorCode::E_MSG_UNKNOWN;
        }
    }

#define SEND_OUT_MSG                                                                     \
    {                                                                                    \
        int32_t msg_size = p_msg->size();                                                \
        int32_t buf_size = p_out_buffer->write_size();                                   \
        if (msg_size <= buf_size) {                                                      \
            int32_t size = p_msg->serialize(p_out_buffer->write_raw_buffer(), buf_size); \
            p_out_buffer->write_consume(size);                                           \
            p_out_msg = nullptr;                                                         \
            p_in_msg = nullptr;                                                          \
        }                                                                                \
    }

    if (p_out_msg) {
        switch (p_out_msg->msg_type) {
        case MsgType::ACCOUNT_RESP: {
            std::shared_ptr<MsgAccountResp> p_msg = std::static_pointer_cast<MsgAccountResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::HOST_RESP: {
            std::shared_ptr<MsgHostResp> p_msg = std::static_pointer_cast<MsgHostResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        default:
            return ErrorCode::E_MSG_UNKNOWN;
        }
    }

    return ErrorCode::NONE;
}

ErrorCode NameNodeSession::writeMsgHandler()
{
    return ErrorCode::NONE;
}

}