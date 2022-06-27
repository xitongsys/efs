#include <iostream>

#include "CliHandlers.h"
#include "Global.h"

namespace efs {

	void CliHandlers::loginHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 5) {
			CliHandlers::wrongParas();
			return;
		}

		Global::config.namenode_ip = tokens[1];
		Global::config.namenode_port = std::stoi(tokens[2]);
		Global::config.user = tokens[3];
		Global::config.password = tokens[4];

		Global::p_client = std::make_shared<efs::Client>(Global::config);
	}

	void CliHandlers::lsHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 2) {
			CliHandlers::wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string path = tokens[1];
		std::vector<FileDesc> fdescs;
		if ((ec = Global::p_client->ls(path, fdescs))) {
			errorHandler(ec);
			return;
		}

		for (const FileDesc& fdesc : fdescs) {
			std::cout << fdesc.path << std::endl;
		}
	}

	void CliHandlers::rmHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 2) {
			wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string path = tokens[1];
		if ((ec = Global::p_client->rm(path))) {
			errorHandler(ec);
			return;
		}
	}

	void CliHandlers::mkdirHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 2) {
			wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string path = tokens[1];
		if ((ec = Global::p_client->mkdir(path))) {
			errorHandler(ec);
			return;
		}
	}

	void CliHandlers::mvHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 3) {
			wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string from_path = tokens[1], to_path = tokens[2];
		if ((ec = Global::p_client->mv(from_path, to_path))) {
			errorHandler(ec);
			return;
		}
	}

	void CliHandlers::permHandler(const std::vector<std::string>& tokens)
	{
	}

	void CliHandlers::hostsHandler(const std::vector<std::string>& tokens)
	{
	}

	void CliHandlers::usersHandler(const std::vector<std::string>& tokens)
	{
	}

	void CliHandlers::wrongParas()
	{
		std::cout << "wrong parameters" << std::endl;
	}

	void CliHandlers::errorHandler(efs::ErrorCode ec)
	{
		std::cout << "error: " << int(ec) << std::endl;
	}

}
