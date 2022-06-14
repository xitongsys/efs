#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "Buffer.h"
#include "Msg/Msg.h"

namespace efs {
class SessionBase : public std::enable_shared_from_this<SessionBase> {
public:
    std::unique_ptr<Buffer> p_in_buffer, p_out_buffer;

    boost::asio::ip::tcp::socket socket;

public:
    ErrorCode writeMsgHandler();
    ErrorCode readMsgHandler();
    void start();

private:
    void do_read();
    void do_write();

public:
    SessionBase(int32_t buffer_size, boost::asio::ip::tcp::socket socket);
};
}