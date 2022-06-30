#pragma once

#ifdef _MSC_VER
#include "NetdiskWin.h"
#define Netdisk NetdiskWin
#else
#include "NetdiskLinux.h"
#define Netdisk NetdiskLinux
#endif

