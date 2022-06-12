#include "DataNodeSession.h"

namespace efs {
DataNodeSession::DataNodeSession(int32_t buffer_size,
    boost::asio::ip::tcp::socket socket)
    : SessionBase(buffer_size, std::move(socket))
{
    p_in_msg = nullptr;
    p_out_msg = nullptr;
}

void DataNodeSession::readHandler()
{
    int32_t size = p_in_buffer->read_size();
    if (size == 0 || p_in_msg != nullptr) {
        return;
    }

    const char* buf = p_in_buffer->read_raw_buffer();

    switch (buf[0]) {
    }
}

void DataNodeSession::writeHandler()
{
}

}