#include "DataNodeSession.h"
#include "Msg/MsgLs.h"

namespace efs {
DataNodeSession::DataNodeSession(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket,
    std::shared_ptr<DataNodeExecutor> p_executor)
    : SessionBase(buffer_size, std::move(socket))
{
    uid = -1;
    gid = -1;
    p_in_msg = nullptr;
    p_out_msg = nullptr;
    this->p_executor = p_executor;
}

void DataNodeSession::readMsgHandler()
{
    int32_t size = p_in_buffer->read_size();
    if (size == 0 || p_in_msg != nullptr) {
        return;
    }

    if (p_in_msg == nullptr) {
        const char* buf = p_in_buffer->read_raw_buffer();
        switch (buf[0]) {
        case MsgType::LS: {
            std::shared_ptr<MsgLs> p_msg = std::make_shared<MsgLs>();
            int n = p_msg->deserialize(buf, size);
            if (n > 0) {
                p_in_msg = p_msg;
                p_in_buffer->read_consume(n);
            }
            break;
        }
        default:
            break;
        }
    }

    if (p_in_msg && p_out_msg == nullptr) {
        switch (p_in_msg->msg_type) {
        case MsgType::LS: {
            ls();
            break;
        }
        default:
            break;
        }
    }

    if (p_out_msg) {
        int32_t msg_size = p_out_msg->size();
        int32_t buf_size = p_out_buffer->write_size();
        if (msg_size <= buf_size) {
            int32_t size = p_out_msg->serialize(p_out_buffer->write_raw_buffer(), buf_size);
            p_out_buffer->write_consume(size);

            p_out_msg = nullptr;
            p_in_msg = nullptr;
        }
    }
}

void DataNodeSession::writeMsgHandler()
{
}

}