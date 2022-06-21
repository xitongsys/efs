#include "DataNode/DataNode.h"
#include "Limit.h"

namespace efs {

DataNode::DataNode(const DataNodeConfig& config)
    : ServerBase<DataNodeSession>(config.ip, config.port)
    , config(config)
{
    p_executor = std::make_shared<DataNodeExecutor>(config);
}

std::shared_ptr<DataNodeSession> DataNode::new_session(boost::asio::ip::tcp::socket socket)
{
    return std::make_shared<DataNodeSession>(int32_t(EFS_BUFFER_SIZE), std::move(socket), p_executor);
}

}