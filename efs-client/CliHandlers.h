#pragma once

#include <vector>
#include <string>
#include <map>

#include "Error.h"

namespace efs {

	class CliHandlers
	{
	public:
		static std::map<std::string, std::string> helpTexts;

	public:
		static void loginHandler(const std::vector<std::string>& tokens);
		static void lsHandler(const std::vector<std::string>& tokens);
		static void rmHandler(const std::vector<std::string>& tokens);
		static void mkdirHandler(const std::vector<std::string>& tokens);
		static void mvHandler(const std::vector<std::string>& tokens);
		static void permHandler(const std::vector<std::string>& tokens);
		static void cpHandler(const std::vector<std::string>& tokens);
		static void mountHandler(const std::vector<std::string>& tokens);
		static void unmountHandler(const std::vector<std::string>& tokens);
		static void infoHandler(const std::vector<std::string>& tokens);
		static void helpHandler(const std::vector<std::string>& tokens);
		static void testHandler(const std::vector<std::string>& tokens);

	public:
		static void wrongParas();
		static void errorHandler(efs::ErrorCode ec);

	};

}

