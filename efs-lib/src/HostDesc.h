#pragma once

#include <string>
#include <vector>

#include "Serialize.h"

namespace efs {

	enum HostType : uint8_t {
		NameNodeHost = 0,
		DataNodeHost = 1,
		ClientHost = 2,
	};

	struct HostDesc {
		HostType host_type;
		std::string name;
		std::string token;
		std::string ip;
		uint16_t port;

		std::vector<std::string> paths;

		int64_t timestamp;

		HostDesc();

		HostDesc(const HostDesc& hdesc);

		HostDesc(HostDesc&& hdesc);

		HostDesc& operator=(const HostDesc& hdesc);

		inline int32_t size() const
		{
			int32_t res = 0;
			res += serialize::size(host_type);
			res += serialize::size(name);
			res += serialize::size(token);
			res += serialize::size(ip);
			res += serialize::size(port);
			res += serialize::size(paths);
			res += serialize::size(timestamp);
			return res;
		}

		inline int32_t serialize(char* buf, int32_t buf_size) const
		{
			if (this->size() > buf_size) {
				return -1;
			}
			int32_t size = 0;
			size += serialize::serialize(host_type, buf + size, buf_size - size);
			size += serialize::serialize(name, buf + size, buf_size - size);
			size += serialize::serialize(token, buf + size, buf_size - size);
			size += serialize::serialize(ip, buf + size, buf_size - size);
			size += serialize::serialize(port, buf + size, buf_size - size);
			size += serialize::serialize(paths, buf + size, buf_size - size);
			size += serialize::serialize(timestamp, buf + size, buf_size - size);
			return size;
		}

		inline int32_t deserialize(const char* buf, int32_t buf_size)
		{
			int size = 0, size1 = 0;

			if ((size1 = serialize::deserialize(host_type, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(name, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(token, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(ip, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(port, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(paths, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			if ((size1 = serialize::deserialize(timestamp, buf + size, buf_size - size)) < 0) {
				return -1;
			}
			size += size1;

			return size;
		}
	};
}
