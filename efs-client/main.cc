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
	while (1) {
		char* line = readline("> ");
		add_history(line);
		std::vector<std::string> tokens = LineParser(std::string(line)).Parse();
		free(line);

		if (tokens.size() == 0) {
			continue;
		}

		std::string cmd = tokens[0];
		if (cmd == "login") {
			efs::CliHandlers::loginHandler(tokens);
		}
		else {
			if (efs::Global::p_client == nullptr) {
				std::cout << "no client" << std::endl;
				continue;
			}

			if (cmd == "ls") {
				efs::CliHandlers::lsHandler(tokens);
			}
			else if (cmd == "mkdir") {
				efs::CliHandlers::mkdirHandler(tokens);
			}
			else if (cmd == "rm") {
				efs::CliHandlers::rmHandler(tokens);
			}
			else if (cmd == "mv") {
				efs::CliHandlers::mvHandler(tokens);
			}
			else if (cmd == "perm") {
				efs::CliHandlers::permHandler(tokens);
			}
			else if (cmd == "cp") {
				efs::CliHandlers::cpHandler(tokens);
			}
			else if (cmd == "mount") {
				efs::CliHandlers::mountHandler(tokens);
			}
			else if (cmd == "unmount") {
				efs::CliHandlers::unmountHandler(tokens);
			}
			else if (cmd == "info") {
				efs::CliHandlers::infoHandler(tokens);
			}
			else if (cmd == "exit") {
				exit(0);
			}
			else {
				std::cout << "unknown command" << std::endl;
			}
		}
	}

	return 0;
}