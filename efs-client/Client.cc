#include <set>

#include "Client.h"



namespace efs {
	Client::Client(const ClientConfig& config) :
		config(config)
	{
		getDataNodes();
	}

	ErrorCode Client::getDataNodes()
	{
		ErrorCode ec = ErrorCode::NONE;
		NameNodeConn conn(io_context, config.namenode_ip, config.namenode_port, "");
		if ((ec = conn.hosts(config.user, config.password, hosts, udesc))) {
			return ec;
		}

		for (auto& p : p_conns) {
			auto& p_conn = std::get<2>(p);
			if (p_conn) {
				p_conn->closeConn();
			}
		}
		p_conns.clear();

		for (int i = 0; i < hosts.size(); i++) {
			std::shared_ptr<DataNodeConn> p_conn = std::make_shared<DataNodeConn>(
				io_context,
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

				p_conns.push_back({ path, i, p_conn });
			}
		}

		std::sort(p_conns.begin(), p_conns.end(), [&](const auto& a, const auto& b) {
			return std::get<0>(a).size() > std::get<0>(b).size();
			});

		return ec;
	}

	ErrorCode Client::getDataNodeConn(const std::string& path, std::shared_ptr<DataNodeConn>& p_conn)
	{
		for (auto it = p_conns.begin(); it != p_conns.end(); it++) {
			const std::string& pre = std::get<0>(*it);
			if ((pre.size() <= path.size() && pre == path.substr(0, pre.size())) ||
				pre.size() == path.size() + 1 && pre == (path + "/")) {
				p_conn = std::get<2>(*it);
				return ErrorCode::NONE;
			}
		}
		return ErrorCode::E_NOT_FOUND;

	}

	ErrorCode Client::getAllDataNodeConns(std::vector<std::shared_ptr<DataNodeConn>>& p_conns)
	{
		std::set<std::shared_ptr<DataNodeConn>> st;
		for (auto it = this->p_conns.begin(); it != this->p_conns.end(); it++) {
			st.insert(std::get<2>(*it));
		}

		p_conns = std::vector<std::shared_ptr<DataNodeConn>>(st.begin(), st.end());
		return ErrorCode::NONE;
	}
}