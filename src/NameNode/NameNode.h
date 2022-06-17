#pragma once

#include <boost/asio.hpp>

#include "NameNode/NameNodeConfig.h"
#include "NameNode/NameNodeSession.h"
#include "ServerBase.h"

namespace efs {

class NameNodeSession;

class NameNode : public ServerBase<NameNodeSession> {
public:
    NameNodeConfig config;

public:
    NameNode(const NameNodeConfig& config);
    std::shared_ptr<NameNodeSession> new_session(boost::asio::ip::tcp::socket socket);
};
}