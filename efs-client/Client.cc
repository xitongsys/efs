#include "Client.h"

namespace efs {
	Client::Client(const ClientConfig& config):
		config(config)
	{
	}

	ErrorCode Client::getDataNodes()
	{
		ErrorCode ec = ErrorCode::NONE;
		NameNodeConn conn(io_context, config.namenode_ip, config.namenode_port, "");
		if ((ec = conn.hosts(hosts))) {
			return ec;
		}

		for (auto it = p_conns.begin(); it != p_conns.end(); it++) {
			if (it->second) {
				it->second->close();
			}
		}
		p_conns.clear();

		for (int i = 0; i < hosts.size(); i++) {
			std::shared_ptr<DataNodeConn> p_conn = std::make_shared<DataNodeConn>(io_context,
				hosts[i].ip, hosts[i].port,
				config.user, config.password);

			if ((ec = p_conn->login())) {
				continue;
			}

			for (std::string path : hosts[i].paths) {
				int n = path.size();
				if (path[n - 1] != '/') {
					path = path + "/";
				}
				p_conns[path] = p_conn;
			}
		}

		return ec;
	}

	ErrorCode Client::getDataNodeConn(const std::string& path, std::shared_ptr<DataNodeConn>& p_conn)
	{
		for (auto it = p_conns.begin(); it != p_conns.end(); it++) {
			const std::string& pre = it->first;
			if ((pre.size() <= path.size() && pre == path.substr(0, pre.size())) ||
				pre.size() == path.size() + 1 && pre == (path + "/")) {
				p_conn = it->second;
				return ErrorCode::NONE;
			}
		}

	}
}