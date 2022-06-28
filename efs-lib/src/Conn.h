#pragma once

#include <iostream>
#include <string>
#include <stdint.h>

#include <boost/asio.hpp>

#include "Error.h"
#include "Serialize.h"
#include "Limit.h"

namespace efs {
	class Conn
	{
	public:
		boost::asio::io_context& io_context;
		boost::asio::ip::tcp::socket sock;
		boost::asio::ip::tcp::resolver resolver;

		std::string addr;
		uint16_t port;
		char *buf;



	public:
		Conn(boost::asio::io_context& io_context, const std::string& addr, uint16_t port);
		~Conn();

		ErrorCode openConn();
		ErrorCode closeConn();

		template<class MSG, class MSG_RESP>
		void query(const MSG& msg, MSG_RESP& msg_resp)
		{
			int32_t msg_size = msg.size();
			if (serialize::serialize(msg_size, buf, EFS_BUFFER_SIZE) <= 0) {
				msg_resp.error_code = ErrorCode::E_SERIALIZE;
				return;
			}

			if (msg.serialize(buf + 4, EFS_BUFFER_SIZE - 4) <= 0) {
				msg_resp.error_code = ErrorCode::E_SERIALIZE;
				return;
			}

			boost::system::error_code b_ec;
			boost::asio::write(sock, boost::asio::buffer(buf, msg_size + 4), b_ec);
			if (b_ec) {
				msg_resp.error_code = ErrorCode::E_NETWORK;
				return;
			}

			int32_t msg_resp_size = 0;
			boost::asio::read(sock, boost::asio::buffer(buf, 4), b_ec);
			if (b_ec) {
				msg_resp.error_code = ErrorCode::E_NETWORK;
				return;
			}

			if (serialize::deserialize(msg_resp_size, buf, 4) <= 0) {
				msg_resp.error_code = ErrorCode::E_DESERIALIZE;
				return;
			}

			if (msg_resp_size > EFS_BUFFER_SIZE) {
				msg_resp.error_code = ErrorCode::E_OVERFLOW;
				return;
			}

			boost::asio::read(sock, boost::asio::buffer(buf, msg_resp_size), b_ec);
			if (b_ec) {
				msg_resp.error_code = ErrorCode::E_NETWORK;
				return;
			}

			if (msg_resp.deserialize(buf, msg_resp_size) < 0) {
				msg_resp.error_code = E_DESERIALIZE;
				return;
			}
		}
	};

}

