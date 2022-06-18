#pragma once

#include <string>
#include <stdint.h>

#include <boost/asio.hpp>

#include "Error.h"
#include "Serialize.h"

namespace efs {
	class Conn
	{
	public:
		static const int32_t BUF_SIZE = 4096;
	public:
		boost::asio::io_context& io_context;
		boost::asio::ip::tcp::socket sock;
		boost::asio::ip::tcp::resolver resolver;

		std::string ip;
		uint16_t port;



	public:
		Conn(boost::asio::io_context& io_context, const std::string& ip, uint16_t port);

		template<class MSG, class MSG_RESP>
		ErrorCode query(const MSG& msg, MSG_RESP& msg_resp)
		{
			char buf[BUF_SIZE];
			int32_t msg_size = msg.size();
			if (serialize::serialize(msg_size, buf, BUF_SIZE) <= 0) {
				return ErrorCode::E_SERIALIZE;
			}

			if (msg.serialize(buf + 4, BUF_SIZE - 4) <= 0) {
				return ErrorCode::E_SERIALIZE;
			}

			boost::asio::write(sock, boost::asio::buffer(buf, msg_size + 4));

			int32_t msg_resp_size = 0;
			boost::asio::read(sock, boost::asio::buffer(buf, 4));
			if (serialize::deserialize(msg_resp_size, buf, 4) <= 0) {
				return ErrorCode::E_DESERIALIZE;
			}

			if (msg_resp_size > BUF_SIZE) {
				return ErrorCode::E_OVERFLOW;
			}

			boost::asio::read(sock, boost::asio::buffer(buf, msg_resp_size));

			if (msg_resp.deserialize(buf, msg_resp_size) < 0) {
				return ErrorCode::E_DESERIALIZE;
			}

			return ErrorCode::NONE;
		}
	};

}

