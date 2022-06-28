#include <algorithm>
#include <cmath>
#include <iostream>

#include "Error.h"
#include "Msg/NameNode/MsgAccount.h"
#include "Msg/NameNode/MsgHost.h"
#include "NameNode/NameNodeSession.h"

namespace efs {
	void NameNodeSession::account()
	{
		std::shared_ptr<MsgAccount> p_in_msg = std::static_pointer_cast<MsgAccount>(this->p_in_msg);
		std::shared_ptr<MsgAccountResp> p_out_msg = std::make_shared<MsgAccountResp>();

		do {
			if (p_in_msg->hdesc.host_type == HostType::ClientHost) {
				p_out_msg->users = namenode.config.users;
				p_out_msg->groups = namenode.config.groups;

				for (auto& user : p_out_msg->users) {
					user.password = "";
				}
			}
			else if (p_in_msg->hdesc.host_type == HostType::DataNodeHost && namenode.config.tokens.count(p_in_msg->hdesc.token) > 0) {
				p_out_msg->users = namenode.config.users;
				p_out_msg->groups = namenode.config.groups;


				std::string ip = socket.remote_endpoint().address().to_string();
				std::string key = p_in_msg->hdesc.name;

				namenode.hosts[key] = p_in_msg->hdesc;
				namenode.hosts[key].ip = ip;
			}
			else {
				p_out_msg->error_code = ErrorCode::E_NOT_FOUND;
				break;
			}

		} while (0);

		this->p_out_msg = p_out_msg;
	}

	void NameNodeSession::host()
	{
		std::shared_ptr<MsgHost> p_in_msg = std::static_pointer_cast<MsgHost>(this->p_in_msg);
		std::shared_ptr<MsgHostResp> p_out_msg = std::make_shared<MsgHostResp>();

		p_out_msg->error_code = ErrorCode::E_USER_NOT_FOUND;
		for (const UserDesc& udesc : namenode.config.users) {
			if (udesc.user == p_in_msg->user && udesc.password == p_in_msg->password) {
				p_out_msg->error_code = ErrorCode::NONE;
				p_out_msg->udesc = udesc;
				break;
			}
		}

		if (p_out_msg->error_code == ErrorCode::NONE) {
			for (auto it = namenode.hosts.begin(); it != namenode.hosts.end(); it++) {
				HostDesc hdesc = it->second;
				// clear the token info
				hdesc.token = "";
				p_out_msg->hosts.push_back(hdesc);
			}
		}

		this->p_out_msg = p_out_msg;
	}

}
