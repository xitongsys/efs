#include "DBBase.h"
#include "Error.h"

namespace efs {

DBBase::DBBase(const std::string& db_path)
{
    int ec = sqlite3_open(db_path.c_str(), &db);
    if (ec) {
        throw ErrorCode::E_DB_OPEN_FAILED;
    }

}

DBBase::~DBBase()
{
    sqlite3_close(db);
}

}