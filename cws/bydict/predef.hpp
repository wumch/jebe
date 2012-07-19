
#pragma once

#define CS_USE_WCS					0
#include "staging.hpp"
#include <boost/asio.hpp>
extern "C" {
#include <limits.h>
}
#define _JEBE_NO_REWIND_OPTI		0
#define _JEBE_SCAN_FROM_RIGHT		0
#define _JEBE_USE_TIMER				0

namespace jebe {
namespace cws {

typedef unsigned char 	byte_t;
typedef uint32_t 		tsize_t;
typedef uint8_t 		wsize_t;
typedef uint32_t 		atimes_t;

}
}
