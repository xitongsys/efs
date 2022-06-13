#pragma once

namespace efs {
enum ErrorCode : int8_t {
    E_OVERFLOW,

    E_DB_OPEN_FAILED,

    E_LOGIN_ACCOUNT_NOT_FOUND,
    E_LOGIN_WRONG_PASSWORD,
};
}
