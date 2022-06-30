#include <iostream>
#include <memory>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>

#include "ClientConfig.h"
#include "Netdisk.h"
#include "CliHandlers.h"
#include "Global.h"
#include "LineParser.h"

int main(int argc, char* argv[])
{
	std::string config_file = "ClientConfig.yaml";
	if (argc > 1) {
		config_file = std::string(argv[1]);
	}

	efs::Global::config = efs::ClientConfig(config_file);

	std::string icon =
		"  ______     ______   ______     \n"
		" /\\  ___\\   /\\  ___\\ /\\  ___\\    \n"
		" \\ \\  __\\   \\ \\  __\\ \\ \\___  \\   \n"
		"  \\ \\_____\\  \\ \\_\\    \\/\\_____\\  \n"
		"   \\/_____/   \\/_/     \\/_____/  \n"
		"\n"
		;

	std::cout << icon << std::endl;

	std::cout << efs::CliHandlers::infoHandler({}) << std::endl;
	std::cout << efs::CliHandlers::loginHandler({}) << std::endl;
	std::cout << efs::CliHandlers::mountHandler({}) << std::endl;

	while (1) {
		std::cout << efs::Global::config.user << "@efs " << efs::Global::pwd << std::endl;
		char* line = readline("> ");
		add_history(line);
		std::vector<std::string> tokens = LineParser(std::string(line)).Parse();
		free(line);

		if (tokens.size() == 0) {
			continue;
		}

		std::string cmd = tokens[0];
		if (cmd == "login") {
			std::cout << efs::CliHandlers::loginHandler(tokens) << std::endl;
		}
		else if (cmd == "help") {
			std::cout << efs::CliHandlers::helpHandler(tokens) << std::endl;
		}
		else if (cmd == "info") {
			std::cout << efs::CliHandlers::infoHandler(tokens) << std::endl;
		}
		else if (cmd == "clear") {
			std::cout << efs::CliHandlers::clearHandler(tokens) << std::endl;
		}
		else if (cmd == "exit") {
			exit(0);
		}
		else {
			if (efs::Global::p_client == nullptr) {
				std::cout << "please login first" << std::endl << std::endl;
				continue;
			}

			if (cmd == "ls") {
				std::cout << efs::CliHandlers::lsHandler(tokens) << std::endl;
			}
			else if (cmd == "cd") {
				std::cout << efs::CliHandlers::cdHandler(tokens) << std::endl;
			}
			else if (cmd == "pwd") {
				std::cout << efs::CliHandlers::pwdHandler(tokens) << std::endl;
			}
			else if (cmd == "mkdir") {
				std::cout << efs::CliHandlers::mkdirHandler(tokens) << std::endl;
			}
			else if (cmd == "rm") {
				std::cout << efs::CliHandlers::rmHandler(tokens) << std::endl;
			}
			else if (cmd == "mv") {
				std::cout << efs::CliHandlers::mvHandler(tokens) << std::endl;
			}
			else if (cmd == "perm") {
				std::cout << efs::CliHandlers::permHandler(tokens) << std::endl;
			}
			else if (cmd == "chown") {
				std::cout << efs::CliHandlers::chownHandler(tokens) << std::endl;
			}
			else if (cmd == "cp") {
				std::cout << efs::CliHandlers::cpHandler(tokens) << std::endl;
			}
			else if (cmd == "mount") {
				std::cout << efs::CliHandlers::mountHandler(tokens) << std::endl;
			}
			else {
				std::cout << "unknown command" << std::endl;
			}
		}
	}

	return 0;
}