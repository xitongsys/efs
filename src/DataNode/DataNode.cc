#include "DataNode/DataNode.h"

namespace efs {

DataNode::DataNode(const std::string& config_file)
    : config(config_file)
    , ServerBase(config.ip, config.port)
{
}

}