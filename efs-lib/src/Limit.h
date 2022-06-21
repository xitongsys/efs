#pragma once

#include <stdint.h>

namespace efs {
	const int32_t EFS_BUFFER_SIZE = 1024 * 1024 * 8;
	const int32_t EFS_MAX_READ_SIZE = EFS_BUFFER_SIZE >> 1;
}