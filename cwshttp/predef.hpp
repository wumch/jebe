
#pragma once

#ifndef CS_DEBUG
#	ifndef NDEBUG
#		define CS_DEBUG				1
#	else
#		define CS_DEBUG				0
#	endif
#endif
#define CS_LOG_ON					0

#define CS_USE_WCS					0

#define BOOST_EXCEPTION_DIABLE		// no guy should throw exceptions to me!

#include "staging.hpp"
#include <boost/static_assert.hpp>
extern "C" {
#include <limits.h>
}
#include "unit.hpp"

#define _JEBE_NO_REWIND_OPTI		0
#define _JEBE_SCAN_FROM_RIGHT		0
#define _JEBE_USE_TIMER				0
#define _JEBE_ENABLE_MAXMATCH		0

#define _JEBE_WORD_MAX_LEN			(8U*3)

#define _JEBE_BUFF_UNIT				1024
#define _JEBE_SESS_RBUF_UNIT		_JEBE_BUFF_UNIT
#define _JEBE_SESS_SBUF_UNIT		_JEBE_SESS_RBUF_UNIT

#define _JEBE_SESS_POOL_INC_STEP	128
#define _JEBE_SESS_POOL_MAX_SIZE	1024

#define _JEBE_HTTP_LINE_SEP 		"\r\n"
#define _JEBE_HTTP_SEP 				_JEBE_HTTP_LINE_SEP _JEBE_HTTP_LINE_SEP

namespace jebe {
namespace cwshttp {

typedef unsigned char 	byte_t;
typedef uint32_t 		tsize_t;	// text-size type.
typedef uint8_t 		wsize_t;	// word-size type.
typedef uint32_t 		atimes_t;	// appear-times of word type.

}
}
