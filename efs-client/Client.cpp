#include "Client.h"

namespace efs {
	Client::Client() 
	{
	}

	ErrorCode Client::getDataNodes()
	{
		NameNodeConn conn(io_context, config.namenode_ip, config.namenode_port, "");
		return conn.hosts(datanodes);
	}
}