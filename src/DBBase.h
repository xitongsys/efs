#include <sqlite3.h>
#include <string>

namespace efs {

class DBBase {
public:
    sqlite3* db;

public:
        

public:
    DBBase(const std::string& db_path);
    ~DBBase();
};

}