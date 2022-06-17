#pragma once

#include <boost/asio.hpp>

#include "NameNode/NameNodeConfig.h"
#include "NameNode/NameNodeExecutor.h"
#include "NameNode/NameNodeSession.h"
#include "ServerBase.h"

namespace efs {
class NameNode : public ServerBase<DataNodeSession> {
public:
    NameNodeConfig config;

public:
    DataNode(const NameNodeConfig& config);
    std::shared_ptr<NameNodeSession> new_session(boost::asio::ip::tcp::socket socket);
};
}