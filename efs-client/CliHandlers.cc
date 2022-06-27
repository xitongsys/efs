#include <iostream>
#include <string>
#include <format>
#include <thread>

#include "CliHandlers.h"
#include "Global.h"
#include "FS.h"

namespace efs {

	std::map<std::string, std::string> CliHandlers::helpTexts = {
		std::make_pair("login", "login example:\nlogin namenode_ip namenode_port username password\n\n"),
		std::make_pair("info", "show info\n\n"),
		std::make_pair("mount", "mount to local disk\n\n"),
		std::make_pair("perm", "modifiy permission\nperm /guest/a.txt user user01 rwx\nperm /guest/b.txt group users rwx\n\n"),
		std::make_pair("mkdir", "mkdir path\n\n"),
		std::make_pair("rm", "rm path\n\n"),
		std::make_pair("cp", "cp from_path to_path\n\n"),
		std::make_pair("mv", "mv from_path to_path\n\n"),

	};

	void CliHandlers::loginHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 5) {
			CliHandlers::wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;

		Global::config.namenode_ip = tokens[1];
		Global::config.namenode_port = std::stoi(tokens[2]);
		Global::config.user = tokens[3];
		Global::config.password = tokens[4];

		Global::p_client = std::make_shared<efs::Client>(Global::config);

		if ((ec = Global::p_client->connect())) {
			CliHandlers::errorHandler(ec);
			return;
		}

		Global::p_netdisk = std::make_shared<Netdisk>(Global::p_client);
	}

	void CliHandlers::lsHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 2) {
			CliHandlers::wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string path = fs::formatPath(tokens[1]);
		std::vector<FileDesc> fdescs;
		if ((ec = Global::p_client->ls(path, fdescs))) {
			errorHandler(ec);
			return;
		}

		for (const FileDesc& fdesc : fdescs) {
			std::cout << fdesc.path << "," << Global::modToStr(fdesc.mod) << "," << fdesc.fsize << "," << fdesc.modified_time << std::endl;
		}
	}

	void CliHandlers::rmHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 2) {
			wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string path = fs::formatPath(tokens[1]);
		if ((ec = Global::p_client->rmRecursive(path))) {
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
		std::string path = fs::formatPath(tokens[1]);
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
		std::string from_path = fs::formatPath(tokens[1]), to_path = fs::formatPath(tokens[2]);
		if ((ec = Global::p_client->mv(from_path, to_path))) {
			errorHandler(ec);
			return;
		}
	}

	void CliHandlers::permHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 5) {
			wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string path = fs::formatPath(tokens[1]);
		PermType perm_type = Global::strToPermType(tokens[2]);
		std::string name = tokens[3];
		Permission perm = Global::strToPerm(tokens[4]);

		if ((ec = Global::p_client->perm(path, name, perm_type, Permission(perm), true))) {
			errorHandler(ec);
			return;
		}
	}

	void CliHandlers::cpHandler(const std::vector<std::string>& tokens)
	{
		if (tokens.size() != 3) {
			wrongParas();
			return;
		}

		ErrorCode ec = ErrorCode::NONE;
		std::string from_path = fs::formatPath(tokens[1]), to_path = fs::formatPath(tokens[2]);

		if ((ec = Global::p_client->cpRecursive(from_path, to_path))) {
			errorHandler(ec);
			return;
		}
	}

	void CliHandlers::mountHandler(const std::vector<std::string>& tokens)
	{
		ErrorCode ec = ErrorCode::NONE;
		if (Global::argv[0] == nullptr) {
			Global::argv[0] = new char[100];
			Global::argv[0][0] = 0;
		}

		Global::p_mount_thread = std::make_shared<std::thread>(std::bind(&Netdisk::mount, Global::p_netdisk, Global::argc, Global::argv));
		Global::p_mount_thread->detach();
	}

	void CliHandlers::unmountHandler(const std::vector<std::string>& tokens)
	{
	}

	void CliHandlers::infoHandler(const std::vector<std::string>& tokens)
	{
		std::cout << "--- account ---" << std::endl;
		std::cout << "namenode_ip: " << Global::config.namenode_ip << std::endl;
		std::cout << "namdenode_port: " << Global::config.namenode_port << std::endl;
		std::cout << "user: " << Global::config.user << std::endl;
		std::cout << "password: " << Global::config.password << std::endl;
		std::cout << "---------------" << std::endl;

		std::cout << "--- datanodes ---" << std::endl;
		for (const HostDesc& hdesc : Global::p_client->hosts) {
			std::cout << hdesc.name << "," << hdesc.ip << "," << hdesc.port << std::endl;
			for (const std::string& path : hdesc.paths) {
				std::cout << path << std::endl;
			}
		}
		std::cout << "-----------------" << std::endl;

		std::cout << "--- users ---" << std::endl;
		for (const UserDesc& udesc : Global::p_client->users) {
			std::cout << udesc.user << "," << udesc.uid << "," << udesc.gid << "," << udesc.root_path << std::endl;
		}
		std::cout << "-------------" << std::endl;

		std::cout << "--- groups ---" << std::endl;
		for (const GroupDesc& gdesc : Global::p_client->groups) {
			std::cout << gdesc.group << "," << gdesc.gid << std::endl;
		}
		std::cout << "--------------" << std::endl;
	}

	void CliHandlers::helpHandler(const std::vector<std::string>& tokens)
	{
		for (auto it = helpTexts.begin(); it != helpTexts.end(); it++) {
			std::cout << it->first << std::endl << it->second << std::endl;
		}
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