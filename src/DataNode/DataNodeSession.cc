#include <iostream>

#include "DataNodeSession.h"
#include "Msg/DataNode/MsgChmod.h"
#include "Msg/DataNode/MsgChown.h"
#include "Msg/DataNode/MsgClose.h"
#include "Msg/DataNode/MsgLogin.h"
#include "Msg/DataNode/MsgLs.h"
#include "Msg/DataNode/MsgMkdir.h"
#include "Msg/DataNode/MsgOpen.h"
#include "Msg/DataNode/MsgPerm.h"
#include "Msg/DataNode/MsgRead.h"
#include "Msg/DataNode/MsgRm.h"
#include "Msg/DataNode/MsgWrite.h"

namespace efs {
DataNodeSession::DataNodeSession(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket,
    std::shared_ptr<DataNodeExecutor> p_executor)
    : SessionBase(buffer_size,
        std::move(socket),
        std::bind(&DataNodeSession::readMsgHandler, this),
        std::bind(&DataNodeSession::writeMsgHandler, this))
{
    p_in_msg = nullptr;
    p_out_msg = nullptr;
    this->p_executor = p_executor;
}

DataNodeSession::~DataNodeSession()
{
    for (auto it = open_files.begin(); it != open_files.end(); it++) {
        p_executor->close(it->second);
    }
}

ErrorCode DataNodeSession::readMsgHandler()
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
        case MsgType::LOGIN: {
            std::shared_ptr<MsgLogin> p_msg = std::make_shared<MsgLogin>();
            RECV_IN_MSG(login);
            break;
        }
        case MsgType::LS: {
            std::shared_ptr<MsgLs> p_msg = std::make_shared<MsgLs>();
            RECV_IN_MSG(ls);
            break;
        }
        case MsgType::RM: {
            std::shared_ptr<MsgRm> p_msg = std::make_shared<MsgRm>();
            RECV_IN_MSG(rm);
            break;
        }
        case MsgType::CHOWN: {
            std::shared_ptr<MsgChown> p_msg = std::make_shared<MsgChown>();
            RECV_IN_MSG(chown);
            break;
        }
        case MsgType::CHMOD: {
            std::shared_ptr<MsgChmod> p_msg = std::make_shared<MsgChmod>();
            RECV_IN_MSG(chmod);
            break;
        }
        case MsgType::MKDIR: {
            std::shared_ptr<MsgMkdir> p_msg = std::make_shared<MsgMkdir>();
            RECV_IN_MSG(mkdir);
            break;
        }
        case MsgType::OPEN: {
            std::shared_ptr<MsgOpen> p_msg = std::make_shared<MsgOpen>();
            RECV_IN_MSG(open);
            break;
        }
        case MsgType::CLOSE: {
            std::shared_ptr<MsgClose> p_msg = std::make_shared<MsgClose>();
            RECV_IN_MSG(close);
            break;
        }
        case MsgType::READ: {
            std::shared_ptr<MsgRead> p_msg = std::make_shared<MsgRead>();
            RECV_IN_MSG(read);
            break;
        }
        case MsgType::WRITE: {
            std::shared_ptr<MsgWrite> p_msg = std::make_shared<MsgWrite>();
            RECV_IN_MSG(write);
            break;
        }
        case MsgType::PERM: {
            std::shared_ptr<MsgPerm> p_msg = std::make_shared<MsgPerm>();
            RECV_IN_MSG(perm);
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
        case MsgType::LOGIN_RESP: {
            std::shared_ptr<MsgLoginResp> p_msg = std::static_pointer_cast<MsgLoginResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::LS_RESP: {
            std::shared_ptr<MsgLsResp> p_msg = std::static_pointer_cast<MsgLsResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::RM_RESP: {
            std::shared_ptr<MsgRmResp> p_msg = std::static_pointer_cast<MsgRmResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::CHOWN_RESP: {
            std::shared_ptr<MsgChownResp> p_msg = std::static_pointer_cast<MsgChownResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::CHMOD_RESP: {
            std::shared_ptr<MsgChmodResp> p_msg = std::static_pointer_cast<MsgChmodResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::MKDIR_RESP: {
            std::shared_ptr<MsgMkdirResp> p_msg = std::static_pointer_cast<MsgMkdirResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::OPEN_RESP: {
            std::shared_ptr<MsgOpenResp> p_msg = std::static_pointer_cast<MsgOpenResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::CLOSE_RESP: {
            std::shared_ptr<MsgCloseResp> p_msg = std::static_pointer_cast<MsgCloseResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::READ_RESP: {
            std::shared_ptr<MsgReadResp> p_msg = std::static_pointer_cast<MsgReadResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::WRITE_RESP: {
            std::shared_ptr<MsgWriteResp> p_msg = std::static_pointer_cast<MsgWriteResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::PERM_RESP: {
            std::shared_ptr<MsgPermResp> p_msg = std::static_pointer_cast<MsgPermResp>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        default:
            return ErrorCode::E_MSG_UNKNOWN;
        }
    }

    return ErrorCode::NONE;
}

ErrorCode DataNodeSession::writeMsgHandler()
{
    return ErrorCode::NONE;
}

}