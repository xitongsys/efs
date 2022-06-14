#pragma once

#include <cstring>
#include <rocksdb/db.h>
#include <string>
#include <tuple>
#include <vector>

#include "Error.h"

namespace efs {

class DBBase {
public:
    rocksdb::DB* db;

public:
    ErrorCode put(const std::string& key, const std::string& value);
    ErrorCode get(const std::string& key, std::string& value);
    ErrorCode del(const std::string& key);

    DBBase(const std::string& db_path);
    ~DBBase();
};

}