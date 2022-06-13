#include "DBBase.h"
#include "Error.h"

namespace efs {

DBBase::DBBase(const std::string& db_path)
{
    rocksdb::Options options;
    options.OptimizeLevelStyleCompaction();
    options.create_if_missing = true;
    rocksdb::Status s = rocksdb::DB::Open(options, db_path, &db);

    if (!s.ok()) {
        throw ErrorCode::E_DB_OPEN;
    }
}

ErrorCode DBBase::put(const std::string& key, const std::string& value)
{
    rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, value);
    if (!s.ok()) {
        return ErrorCode::E_DB_PUT;
    }
    return ErrorCode::NONE;
}

ErrorCode DBBase::get(const std::string& key, std::string& value)
{
    rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &value);
    if (!s.ok()) {
        return ErrorCode::E_DB_GET;
    }
    return ErrorCode::NONE;
}

ErrorCode DBBase::del(const std::string& key)
{
    rocksdb::Status s = db->Delete(rocksdb::WriteOptions(), key);
    if (!s.ok()) {
        return ErrorCode::E_DB_DEL;
    }
    return ErrorCode::NONE;
}

DBBase::~DBBase()
{
    delete db;
}

}