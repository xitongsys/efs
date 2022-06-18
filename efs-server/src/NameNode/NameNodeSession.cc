#include <iostream>

#include "Msg/NameNode/MsgAccount.h"
#include "Msg/NameNode/MsgHost.h"
#include "NameNode/NameNodeSession.h"

namespace efs {
NameNodeSession::NameNodeSession(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket,
    NameNode& namenode)
    : SessionBase(buffer_size,
        std::move(socket),
        std::bind(&NameNodeSession::readMsgHandler, this),
        std::bind(&NameNodeSession::writeMsgHandler, this))
    , namenode(namenode)
    , p_in_msg(nullptr)
    , p_out_msg(nullptr)
{
}

NameNodeSession::~NameNodeSession()
{
}

ErrorCode NameNodeSession::readMsgHandler()
{
    int32_t in_buffer_size = p_in_buffer->read_size();
    int32_t read_size = 0;
    if (in_buffer_size <= 4 || p_in_msg != nullptr) {
        return ErrorCode::NONE;
    }

#define RECV_IN_MSG(FUNC)                                                        \
    {                                                                            \
        int n = p_msg->deserialize(buf + read_size, in_buffer_size - read_size); \
        if (n <= 0) {                                                            \
            return ErrorCode::E_DESERIALIZE;                                     \
        }                                                                        \
        read_size += n;                                                          \
        p_in_msg = p_msg;                                                        \
        p_in_buffer->read_consume(read_size);                                    \
        FUNC();                                                                  \
    }

    if (p_in_msg == nullptr) {
        const char* buf = p_in_buffer->read_raw_buffer();
        int32_t msg_size = 0;
        read_size += serialize::deserialize(msg_size, buf + read_size, in_buffer_size - read_size);

        if (msg_size + 4 > in_buffer_size) {
            return ErrorCode::NONE;
        }

        switch ((buf + read_size)[0]) {
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

#define SEND_OUT_MSG                                                                                      \
    {                                                                                                     \
        int32_t msg_size = p_msg->size();                                                                 \
        int32_t out_buffer_size = p_out_buffer->write_size();                                             \
        char* buf = p_out_buffer->write_raw_buffer();                                                     \
        if (msg_size + 4 <= out_buffer_size) {                                                            \
            int32_t write_size = 0;                                                                       \
            write_size += serialize::serialize(msg_size, buf + write_size, out_buffer_size - write_size); \
            write_size += p_msg->serialize(buf + write_size, out_buffer_size - write_size);               \
            p_out_buffer->write_consume(write_size);                                                      \
            p_out_msg = nullptr;                                                                          \
            p_in_msg = nullptr;                                                                           \
        }                                                                                                 \
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