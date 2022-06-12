#include "DataNode/DataNode.h"

namespace efs {

DataNode::DataNode(const std::string& config_file)
    : config(config_file)
{
    boost::system::error_code ec;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(config.ip, ec);

    if (ec.value()) {
        throw ec;
    }

    boost::asio::ip::tcp::endpoint endpoint(ip_address, config.port);

    acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(
        io_context,
        endpoint);
}

void DataNode::do_accept()
{
    acceptor->async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            std::make_shared<DataNodeSession>(std::move(socket))->start();
        }
        do_accept();
    });
}

void DataNode::run()
{
}

}