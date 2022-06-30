#pragma once

#include <array>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <filesystem>

#include "Error.h"

namespace efs {
	namespace fs {
		constexpr std::array<uint8_t, (1 << 8)> CS{
			[]() constexpr {
				std::array<uint8_t, (1 << 8)> res {};
		constexpr char cs[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-.() @!~`#$%^&+={}[]|;,";
		const char* p = cs;
		while (*p) {
			res[*p] = 1;
			p++;
		}
		return res;
	}() };

		inline ErrorCode checkPath(const std::string& path) 
		{
			for (char c : path) {
				if ((CS[c] == 0) && (c != '/') && (c != '\\')) {
					return ErrorCode::E_FILE_INVALID_PATH;
				}
			}
			return ErrorCode::NONE;
		}

		inline std::string format(const std::string& path)
		{
			int n = path.size();
			std::string res;
			for (int i = 0; i < n; i++) {
				char c = path[i];
				if (c == '\\') {
					c = '/';
				}

				if (CS[c]) {
					res.push_back(c);
				}
				else if (c == '/') {
					if (res.size() == 0 || (*res.rbegin()) != '/') {
						res.push_back(c);
					}
				}
			}
			if (res.size() == 0) {
				return "/";
			}

			if (res.size() > 1 && *(res.rbegin()) == '/') {
				res.pop_back();
			}

			return res;
		}

		inline std::string basename(const std::string& path)
		{
			int n = path.size();
			std::string res;
			int i = n - 1;
			while (i >= 0 && path[i] == '/') {
				i--;
			}
			while (i >= 0 && path[i] != '/') {
				res.push_back(path[i]);
				i--;
			}

			reverse(res.begin(), res.end());
			return res;
		}

		inline std::string parent(const std::string& path) {
			std::string cur_path = fs::format(path);
			while (cur_path.size() && (*cur_path.rbegin())!='/') {
				cur_path.pop_back();
			}
			if (cur_path.size() > 1) {
				cur_path.pop_back();
			}
			return cur_path;
		}

		inline std::vector<std::string> split(const std::string& path) {
			std::string cur_path = fs::format(path);
			std::vector<std::string> res;
			int n = cur_path.size();
			if (cur_path.size() == 0) {
				return res;
			}

			if (cur_path[0] == '/') {
				res.push_back("/");
				cur_path = cur_path.substr(1, n - 1);
			}

			int i = 0;
			while (i < n) {
				int j = i;
				while (j < n && cur_path[j] != '/') {
					j++;
				}
				res.push_back(cur_path.substr(i, j - i));
				i = j + 1;
			}
			return res;
		}

		inline std::string join(const std::vector<std::string>& paths) {
			if (paths.size() == 0) {
				return "";
			}

			std::string res = paths[0];
			for (int i = 1; i < int(paths.size()); i++) {
				res += "/" + paths[i];
			}
			return res;
		}



		inline bool exists(const std::string& path)
		{
			return std::filesystem::exists(path);
		}

		inline bool isFile(const std::string& path)
		{
			return std::filesystem::is_regular_file(path);
		}

		inline bool isDirectory(const std::string& path)
		{
			return std::filesystem::is_directory(path);
		}

		inline int64_t fileSize(const std::string& path)
		{
			return std::filesystem::file_size(path);
		}

		inline int64_t modifiedTime(const std::string& path)
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::filesystem::last_write_time(path).time_since_epoch())
				.count();
		}
	}
}