#pragma once

#include <boost/asio.hpp>
#include <iostream>
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

    virtual std::shared_ptr<Session> new_session(boost::asio::ip::tcp::socket socket)
    {
        return nullptr;
    }

    void do_accept()
    {
        acceptor->async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                new_session(std::move(socket))->start();
            }
            do_accept();
        });
    }

    void run()
    {
        do_accept();
        io_context.run();
    }
};
}