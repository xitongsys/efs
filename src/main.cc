#include <iostream>
#include <string>

#include "DataNode/DataNode.h"
#include "DataNode/DataNodeConfig.h"
#include "Error.h"
#include "NameNode/NameNode.h"
#include "NameNode/NameNodeConfig.h"

int main(int argc, char** argv)
{
    std::string role(argv[1]);
    std::string config_file = std::string(argv[2]);

    try {
        if (role == "datanode") {
            efs::DataNodeConfig config(config_file);
            efs::DataNode datanode(config);

            datanode.run();
        }

        if (role == "namenode") {
            efs::NameNodeConfig config(config_file);
            efs::NameNode namenode(config);

            namenode.run();
        }

    } catch (boost::system::error_code ec) {
        std::cout << ec.category().name() << ":" << ec.value() << "," << ec.message() << std::endl;

    } catch (efs::ErrorCode ec) {
        std::cout << ec << std::endl;
    }

    return 0;
}