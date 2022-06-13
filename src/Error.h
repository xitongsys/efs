#pragma once

namespace efs {
enum ErrorCode : int8_t {
    NONE = 0,
    E_OVERFLOW,

    E_LOGIN_USER_NOT_FOUND,
    E_LOGIN_WRONG_PASSWORD,

    E_SERIALIZE,
    E_DESERIALIZE,

    E_DB_OPEN,
    E_DB_GET,
    E_DB_PUT,
    E_DB_DEL,
};
}
