#pragma once

#include <boost/asio.hpp>
#include <queue>

namespace efs {

template <typename Session>
class ServerBase {

private:
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;

public:
    ServerBase(const std::string& ip, int port)
    {
        boost::system::error_code ec;
        boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(ip, ec);

        if (ec.value()) {
            throw ec;
        }

        boost::asio::ip::tcp::endpoint endpoint(ip_address, port);

        acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(
            io_context,
            endpoint);
    }

    void do_accept()
    {
        acceptor->async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->start();
            }
            do_accept();
        });
    }

    void run()
    {
        io_context.run();
    }
};
}