

#pragma once

#define CS_USE_WCS 0
#include "predef.hpp"

#define _JEBE_NO_REWIND_OPTI		0
#define _JEBE_SCAN_FROM_RIGHT		0
#define _JEBE_USE_TIMER				0
#define _JEBE_ENABLE_MAXMATCH		0
#define _JEBE_STEP_FWD_OPTI			1

namespace jebe {
namespace cws {

typedef unsigned char byte_t;
typedef uint32_t tsize_t;
typedef uint8_t wsize_t;

}
}
