#include <string>

#include "HostDesc.h"
#include "Util.h"

namespace efs {
	HostDesc::HostDesc()
	{
		name = "";
		token = "";
		ip = "";
		port = 0;
		paths = {};
	}

	HostDesc::HostDesc(const HostDesc& hdesc)
	{
		host_type = hdesc.host_type;
		name = hdesc.name;
		token = hdesc.token;
		ip = hdesc.ip;
		port = hdesc.port;
		paths = hdesc.paths;
	}

	HostDesc::HostDesc(HostDesc&& hdesc)
		: host_type(hdesc.host_type)
		, name(std::move(hdesc.name))
		, token(std::move(hdesc.token))
		, ip(std::move(hdesc.ip))
		, port(hdesc.port)
		, paths(std::move(hdesc.paths))
	{
	}

	HostDesc& HostDesc::operator=(const HostDesc& hdesc)
	{
		host_type = hdesc.host_type;
		name = hdesc.name;
		token = hdesc.token;
		ip = hdesc.ip;
		port = hdesc.port;
		paths = hdesc.paths;
		return *this;
	}

}
