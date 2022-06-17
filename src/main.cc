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

    if (role == "datanode") {
        try {
            efs::DataNodeConfig config(config_file);
            efs::DataNode datanode(config);

            datanode.run();

        } catch (boost::system::error_code ec) {
            std::cout << ec.category().name() << ":" << ec.value() << "," << ec.message() << std::endl;
        }
    }

    if (role == "namenode") {
        try {
            efs::NameNodeConfig config(config_file);
            efs::NameNode namenode(config);

            namenode.run();

        } catch (boost::system::error_code ec) {
            std::cout << ec.category().name() << ":" << ec.value() << "," << ec.message() << std::endl;
        }
    }

    return 0;
}