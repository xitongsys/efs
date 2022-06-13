#include <unordered_set>

#include "DataNode/DataNodeExecutor.h"
#include "SessionBase.h"

namespace efs {
class DataNodeSession : public SessionBase {
public:
    std::shared_ptr<DataNodeExecutor> p_executor;

    std::shared_ptr<Msg> p_in_msg, p_out_msg;

    std::unordered_set<int64_t> fds;

    int64_t uid, gid;

public:
    DataNodeSession(int32_t buffer_size, boost::asio::ip::tcp::socket socket, std::shared_ptr<DataNodeExecutor> p_executor);

    void readHandler();
    void writeHandler();

    void loginHandler();
    void lsHandler();
    void openHandler();
};
}