#pragma once

namespace efs {
enum ErrorCode : int8_t {
    NONE = 0,
    E_OVERFLOW = -1,
    E_BUFFER_SIZE,

    E_LOGIN_USER_NOT_FOUND = -2,
    E_LOGIN_WRONG_PASSWORD = -3,

    E_SERIALIZE = -4,
    E_DESERIALIZE = -5,

    E_FILE_OPEN = -6,
    E_FILE_RM,
    E_FILE_MKDIR,
    E_FILE_WRITE,
    E_FILE_READ,
    E_FILE_EOF,
    E_FILE_PERMISSION,
    E_FILE_NOT_FOUND,
    E_FILE_TOO_MANY,

    E_DB_OPEN = -7,
    E_DB_GET = -8,
    E_DB_PUT = -9,
    E_DB_DEL = -10,
};
}
