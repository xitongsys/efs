#include <cstring>
#include <sqlite3.h>
#include <string>
#include <tuple>
#include <vector>

#include "Error.h"

namespace efs {

class DBBase {
public:
    sqlite3* db;

    static const char* SQLS;

public:
    template <int SQL_INDEX, class... COL_TYPES>
    std::vector<std::tuple<COL_TYPES...>> select()
    {
        const char* sql = SQLS[SQL_INDEX];
    }

public:
    template <class T>
    ErrorCode parse(std::string& s, T& value)
    {
        if (s.size() != sizeof(T)) {
            return ErrorCode::E_DB_PARSE;
        }
        memcpy(s.c_str(), &value, sizeof(T));
        return ErrorCode::NONE;
    }

    ErrorCode parse(std::string& s, std::string& value)
    {
        value = std::move(s);
        return ErrorCode::NONE;
    }

public:
    DBBase(const std::string& db_path);
    ~DBBase();
};

}