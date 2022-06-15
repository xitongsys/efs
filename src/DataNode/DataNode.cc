#include "DataNode/DataNode.h"

namespace efs {

DataNode::DataNode(const DataNodeConfig& config)
    : config(config)
    , ServerBase<DataNodeSession>(config.ip, config.port)
{
    p_executor = std::make_shared<DataNodeExecutor>(config);
}

std::shared_ptr<DataNodeSession> DataNode::new_session(boost::asio::ip::tcp::socket socket)
{
    return std::make_shared<DataNodeSession>(config.buffer_size, std::move(socket), p_executor);
}

}