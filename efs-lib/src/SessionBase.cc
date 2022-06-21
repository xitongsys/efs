#include <iostream>

#include "SessionBase.h"

namespace efs {
SessionBase::SessionBase(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket,
    std::function<ErrorCode()> read_msg_handler,
    std::function<ErrorCode()> write_msg_handler)
    : socket(std::move(socket))
{
    p_in_buffer = std::make_unique<Buffer>(buffer_size);
    p_out_buffer = std::make_unique<Buffer>(buffer_size);
    this->read_msg_handler = read_msg_handler;
    this->write_msg_handler = write_msg_handler;
}

void SessionBase::start()
{
    do_read();
}

void SessionBase::do_read()
{
    auto self(shared_from_this());
    socket.async_read_some(boost::asio::buffer(p_in_buffer->write_raw_buffer(), p_in_buffer->write_size()),
        [this, self](boost::system::error_code ec, int32_t size) {
            if (!ec && size >= 0) {
                p_in_buffer->write_consume(size);
                ErrorCode ec = read_msg_handler();
                if (!ec) {
                    do_write();
                }
                else {
                    //std::cout << "error" << int(ec) << std::endl;
                }
            }
        });
}

void SessionBase::do_write()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket, boost::asio::buffer(p_out_buffer->read_raw_buffer(), p_out_buffer->read_size()),
        [this, self](boost::system::error_code ec, int32_t size) {
            if (!ec && size >= 0) {
                p_out_buffer->read_consume(size);
                ErrorCode ec = write_msg_handler();
                if (!ec) {
                    do_read();
                }
                else {
                    //std::cout << "error" << int(ec) << std::endl;
                }
            }
        });
}

}