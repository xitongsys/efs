#include <iostream>
#include <string>

#include "DataNode/DataNode.h"

int main(int argc, char** argv)
{
    std::string config_file = std::string(argv[1]);

    efs::DataNode datanode(config_file);

    datanode.run();

    return 0;
}