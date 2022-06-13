#pragma once

namespace efs {
enum ErrorCode : int8_t {
    NONE = 0,
    E_OVERFLOW,

    E_LOGIN_ACCOUNT_NOT_FOUND,
    E_LOGIN_WRONG_PASSWORD,

    E_DB_OPEN,
    E_DB_GET,
    E_DB_PUT,
    E_DB_DEL,
};
}
