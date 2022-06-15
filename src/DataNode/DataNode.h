#pragma once

#include <boost/asio.hpp>

#include "DataNode/DataNodeConfig.h"
#include "DataNode/DataNodeExecutor.h"
#include "DataNode/DataNodeSession.h"
#include "ServerBase.h"

namespace efs {
class DataNode : public ServerBase<DataNodeSession> {
public:
    DataNodeConfig config;
    std::shared_ptr<DataNodeExecutor> p_executor;

public:
    DataNode(const DataNodeConfig& config);
    std::shared_ptr<DataNodeSession> new_session(boost::asio::ip::tcp::socket socket);
};
}