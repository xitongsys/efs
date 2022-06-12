#include <boost/asio.hpp>
#include <queue>

#include "DataNodeConfig.h"
#include "DataNodeSession.h"

namespace efs {
class DataNode {
public:
    DataNodeConfig config;

private:
    boost::asio::io_context io_context;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;

public:
    DataNode(const std::string& config_file);

    void do_accept();
    void run();
};
}