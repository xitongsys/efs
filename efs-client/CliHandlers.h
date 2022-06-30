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
		static std::string loginHandler(const std::vector<std::string>& tokens);
		static std::string lsHandler(const std::vector<std::string>& tokens);
		static std::string cdHandler(const std::vector<std::string>& tokens);
		static std::string pwdHandler(const std::vector<std::string>& tokens);
		static std::string rmHandler(const std::vector<std::string>& tokens);
		static std::string mkdirHandler(const std::vector<std::string>& tokens);
		static std::string mvHandler(const std::vector<std::string>& tokens);
		static std::string permHandler(const std::vector<std::string>& tokens);
		static std::string chownHandler(const std::vector<std::string>& tokens);
		static std::string cpHandler(const std::vector<std::string>& tokens);
		static std::string mountHandler(const std::vector<std::string>& tokens);
		static std::string infoHandler(const std::vector<std::string>& tokens);
		static std::string helpHandler(const std::vector<std::string>& tokens);

	public:
		static std::string absolutePath(const std::string& path);
		static std::string wrongParas();
		static std::string errorHandler(efs::ErrorCode ec);

	};

}

