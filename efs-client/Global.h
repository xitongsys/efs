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


	public:
		static std::string permToStr(uint8_t p);
	};

}

