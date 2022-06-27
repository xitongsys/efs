#include "Global.h"

namespace efs {

	efs::ClientConfig Global::config;
	std::shared_ptr<efs::Client> Global::p_client = nullptr;
	std::shared_ptr<efs::Netdisk> Global::p_netdisk = nullptr;

	std::string Global::permToStr(uint8_t p)
	{
		std::string res = "rwx";
		if ((p & 0b100) == 0) {
			res[0] = '-';
		}

		if ((p & 0b010) == 0) {
			res[1] = '-';
		}

		if ((p & 0b001) == 0) {
			res[2] = '-';
		}
		return res;
	}

}

