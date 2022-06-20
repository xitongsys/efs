#include <iostream>
#include <memory>

#include "ClientConfig.h"
#include "Netdisk.h"

int main(int argc, char* argv[])
{
	efs::ErrorCode ec = efs::ErrorCode::NONE;
	efs::ClientConfig config;
	config.namenode_ip = "127.0.0.1";
	config.namenode_port = 20000;
	config.user = "zxt";
	config.password = "zxtpwd";

	std::shared_ptr<efs::Client> p_client = std::make_shared<efs::Client>(config);

	efs::Netdisk netdisk(p_client);

	netdisk.mount(argc, argv);

	while (1) {
		Sleep(1000);
	}

	return 0;
}