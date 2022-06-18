#include "NameNode/NameNode.h"

namespace efs {

NameNode::NameNode(const NameNodeConfig& config)
    : ServerBase<NameNodeSession>(config.ip, config.port)
    , config(config)
{
}

std::shared_ptr<NameNodeSession> NameNode::new_session(boost::asio::ip::tcp::socket socket)
{
    return std::make_shared<NameNodeSession>(config.buffer_size, std::move(socket), *this);
}

}