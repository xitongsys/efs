#include "DataNodeSession.h"
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
DataNodeSession::DataNodeSession(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket,
    std::shared_ptr<DataNodeExecutor> p_executor)
    : SessionBase(buffer_size, std::move(socket))
{
    p_in_msg = nullptr;
    p_out_msg = nullptr;
    this->p_executor = p_executor;
}

ErrorCode DataNodeSession::readMsgHandler()
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
        case MsgType::LOGIN: {
            std::shared_ptr<MsgLogin> p_msg = std::static_pointer_cast<MsgLogin>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::LS: {
            std::shared_ptr<MsgLs> p_msg = std::static_pointer_cast<MsgLs>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::RM: {
            std::shared_ptr<MsgRm> p_msg = std::static_pointer_cast<MsgRm>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::CHOWN: {
            std::shared_ptr<MsgChown> p_msg = std::static_pointer_cast<MsgChown>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::CHMOD: {
            std::shared_ptr<MsgChmod> p_msg = std::static_pointer_cast<MsgChmod>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::MKDIR: {
            std::shared_ptr<MsgMkdir> p_msg = std::static_pointer_cast<MsgMkdir>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::OPEN: {
            std::shared_ptr<MsgOpen> p_msg = std::static_pointer_cast<MsgOpen>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::CLOSE: {
            std::shared_ptr<MsgClose> p_msg = std::static_pointer_cast<MsgClose>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::READ: {
            std::shared_ptr<MsgRead> p_msg = std::static_pointer_cast<MsgRead>(p_out_msg);
            SEND_OUT_MSG;
            break;
        }
        case MsgType::WRITE: {
            std::shared_ptr<MsgWrite> p_msg = std::static_pointer_cast<MsgWrite>(p_out_msg);
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