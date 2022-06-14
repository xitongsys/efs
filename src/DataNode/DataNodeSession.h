#include <unordered_set>

#include "DataNode/DataNodeExecutor.h"
#include "SessionBase.h"

namespace efs {

class DataNodeSession : public SessionBase {
public:
    static const int32_t MAX_OPEN_FILE_NUM = 1024;

public:
    std::shared_ptr<DataNodeExecutor> p_executor;

    std::shared_ptr<Msg> p_in_msg, p_out_msg;

    std::unordered_map<int32_t, OpenFileHandler> open_files;

    int64_t uid, gid;

public:
    DataNodeSession(int32_t buffer_size,
        boost::asio::ip::tcp::socket socket,
        std::shared_ptr<DataNodeExecutor> p_executor);

    void readMsgHandler();
    void writeMsgHandler();

    void login();
    void ls();
    void rm();
    void chown();
    void chmod();
    void mkdir();

    void open();
    void close();
    void read();
    void write();
};
}