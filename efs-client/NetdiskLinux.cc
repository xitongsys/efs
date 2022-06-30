#include <stack>
#include <string>
#include <tuple>

#include "NetdiskLinux.h"
#include "FS.h"

namespace efs {

	NetdiskLinux::NetdiskLinux(std::shared_ptr<Client>& p_client):
		p_client(p_client)
	{
		buffer = new char[EFS_BUFFER_SIZE];
	}

	NetdiskLinux::~NetdiskLinux()
	{
		delete[] buffer;
	}

	int NetdiskLinux::mount(int argc, char* argv[])
	{
		return -1;
	}
}
