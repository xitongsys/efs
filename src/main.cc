#include <iostream>
#include <string>

#include "DataNode/DataNode.h"
#include "DataNode/DataNodeConfig.h"
#include "Error.h"

int main(int argc, char** argv)
{
    try {
        std::string config_file = std::string(argv[1]);
        efs::DataNodeConfig config(config_file);
        efs::DataNode datanode(config);

        datanode.run();
        
    } catch (boost::system::error_code ec) {
        std::cout << ec.category().name() << ":" << ec.value() << "," << ec.message() << std::endl;
    }

    return 0;
}