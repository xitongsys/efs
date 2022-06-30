#include "Global.h"

namespace efs {

	efs::ClientConfig Global::config;
	std::shared_ptr<efs::Client> Global::p_client = nullptr;
	std::shared_ptr<efs::Netdisk> Global::p_netdisk = nullptr;
	std::shared_ptr<std::thread> Global::p_mount_thread = nullptr;

	int Global::argc = 1;
	char* Global::argv[] = { nullptr };

	std::string Global::pwd = "/";

	std::string Global::permToStr(Permission p)
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

	Permission Global::strToPerm(const std::string& perm_str)
	{
		uint8_t res = 0b000;
		if (perm_str.find('r') != std::string::npos) {
			res |= 0b100;
		}

		if (perm_str.find('w') != std::string::npos) {
			res |= 0b010;
		}

		if (perm_str.find('x') != std::string::npos) {
			res |= 0b001;
		}
		return Permission(res);
	}

	PermType Global::strToPermType(const std::string& perm_type_str) 
	{
		if (perm_type_str == "user") {
			return PermType::USER;
		}
		else if (perm_type_str == "group") {
			return PermType::GROUP;
		}
		else if (perm_type_str == "other") {
			return PermType::OTHER;
		}

		return PermType::DEFAULT_PERMTYPE;
	}

	std::string Global::modToStr(uint16_t mod)
	{
		std::string res = "-";
		if (F_IFDIR & mod) {
			res[0] = 'd';
		}

		res += permToStr(Permission((mod >> 6) & (0b111)));
		res += permToStr(Permission((mod >> 3) & (0b111)));
		res += permToStr(Permission((mod >> 0) & (0b111)));
		return res;
	}

}

