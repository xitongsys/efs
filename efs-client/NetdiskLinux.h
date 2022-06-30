#pragma once

#include <stdint.h>
#include <memory>
#include <unordered_map>

#include "Client.h"
#include "Error.h"


namespace efs {
	class NetdiskLinux {
	public:
		std::mutex mutex;
		std::shared_ptr<Client>& p_client;
		char* buffer;

	public:
		int mount(int argc, char* argv[]);

	public:
		NetdiskLinux(std::shared_ptr<Client>& p_client);
		~NetdiskLinux();
	};

}

