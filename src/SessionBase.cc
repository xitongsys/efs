#include "SessionBase.h"

namespace efs {
SessionBase::SessionBase(int32_t buffer_size, boost::asio::ip::tcp::socket socket)
    : socket(std::move(socket))
{
    p_in_buffer = std::make_unique<Buffer>(buffer_size);
    p_out_buffer = std::make_unique<Buffer>(buffer_size);
}

void SessionBase::readMsgHandler() { }

void SessionBase::writeMsgHandler() { }

void SessionBase::start()
{
    do_read();
}

void SessionBase::do_read()
{
    auto self(shared_from_this());
    socket.async_read_some(boost::asio::buffer(p_in_buffer->write_raw_buffer(), p_in_buffer->write_size()),
        [this, self](boost::system::error_code ec, int32_t size) {
            if (!ec) {
                readMsgHandler();
                do_write();
            }
        });
}

void SessionBase::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket, boost::asio::buffer(p_out_buffer->read_raw_buffer(), p_in_buffer->read_size()),
        [this, self](boost::system::error_code ec, int32_t size) {
            if (!ec) {
                writeMsgHandler();
                do_read();
            }
        });
}

}