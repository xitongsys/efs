#pragma once

#include <memory>

#include "Client.h"
#include "Netdisk.h"

namespace efs {

	class Global
	{
	public:
		static efs::ClientConfig config;
		static std::shared_ptr<efs::Client> p_client;
		static std::shared_ptr<efs::Netdisk> p_netdisk;
		static std::shared_ptr<std::thread> p_mount_thread;

		static int argc;
		static char* argv[];

		static std::string pwd;




	public:
		static std::string permToStr(Permission p);
		static Permission strToPerm(const std::string& perm_str);

		static PermType strToPermType(const std::string& perm_type_str);

		static std::string modToStr(uint16_t mod);
	};

}

