#pragma once

namespace efs {
enum ErrorCode : int8_t {
    NONE = 0,
    E_OVERFLOW = -127,
    E_BUFFER_SIZE,

    E_PANIC,

    E_MSG_UNKNOWN,

    E_LOGIN_USER_NOT_FOUND,
    E_LOGIN_WRONG_PASSWORD,

    E_USER_NOT_FOUND,
    E_GROUP_NOT_FOUND,

    E_SERIALIZE,
    E_DESERIALIZE,

    E_FILE_OPEN,
    E_FILE_RM,
    E_FILE_MV,
    E_FILE_MKDIR,
    E_FILE_LS,
    E_FILE_WRITE,
    E_FILE_READ,
    E_FILE_EOF,
    E_FILE_PERMISSION,
    E_FILE_NOT_FOUND,
    E_FILE_TOO_MANY,
    E_FILE_PATH,
    E_FILE_PERM,
    E_FILE_SEEK,
    E_FILE_TRUNCATE,

    E_DB_OPEN,
    E_DB_GET,
    E_DB_PUT,
    E_DB_DEL,

    E_TOKEN_ERROR,
    E_CONFIG_ERROR,
};
}
