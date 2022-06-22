#pragma once

#include <memory>
#include <unordered_set>

#include "DataNode/DataNodeExecutor.h"
#include "SessionBase.h"
#include "UserDesc.h"

namespace efs {

class DataNodeSession : public SessionBase {
public:
    static const int32_t MAX_OPEN_FILE_NUM = 1024;

public:
    std::shared_ptr<DataNodeExecutor> p_executor;

    std::shared_ptr<Msg> p_in_msg, p_out_msg;

    std::unordered_map<int32_t, OpenFileHandler> open_files;

    UserDesc udesc;

public:
    DataNodeSession(int32_t buffer_size,
        boost::asio::ip::tcp::socket socket,
        std::shared_ptr<DataNodeExecutor> p_executor);

    ~DataNodeSession();

    ErrorCode readMsgHandler();
    ErrorCode writeMsgHandler();

    void login();
    void getFileDesc();
    void ls();

    //can only rm file or empty dir
    void rm();
    //can only mv file or empty dir
    void mv();

    void chown();
    void chmod();
    void mkdir();
    void perm();

    void open();
    void close();
    void read();
    void write();

    void openOffset();
    void readOffset();
    void writeOffset();
    void truncate();
};
}