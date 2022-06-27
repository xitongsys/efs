#pragma once

#include <vector>
#include <string>

#include "Error.h"

namespace efs {

	class CliHandlers
	{
	public:
		static void loginHandler(const std::vector<std::string>& tokens);
		static void lsHandler(const std::vector<std::string>& tokens);
		static void rmHandler(const std::vector<std::string>& tokens);
		static void mkdirHandler(const std::vector<std::string>& tokens);
		static void mvHandler(const std::vector<std::string>& tokens);
		static void permHandler(const std::vector<std::string>& tokens);
		static void hostsHandler(const std::vector<std::string>& tokens);
		static void usersHandler(const std::vector<std::string>& tokens);

	public:
		static void wrongParas();
		static void errorHandler(efs::ErrorCode ec);

	};

}

