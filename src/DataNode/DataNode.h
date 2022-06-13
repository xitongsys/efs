#include <boost/asio.hpp>
#include <queue>

#include "DataNode/DataNodeConfig.h"
#include "DataNode/DataNodeExecutor.h"
#include "DataNode/DataNodeSession.h"
#include "ServerBase.h"

namespace efs {
class DataNode : public ServerBase<DataNodeSession> {
public:
    DataNodeConfig config;
    std::shared_ptr<DataNodeExecutor> p_executor;

private:
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;

public:
    DataNode(const std::string& config_file);
};
}