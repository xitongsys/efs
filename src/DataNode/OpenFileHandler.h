#include <cstdio>
#include <stdint.h>

#include "FileDesc.h"

namespace efs {

struct OpenFileHandler {
    int32_t fd;
    FILE* fp;
    FileDesc fdesc;
};

}