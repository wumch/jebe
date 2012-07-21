
#pragma once

#define CS_DEBUG					1
#define CS_LOG_ON					1

#define CS_USE_WCS					0

#include "staging.hpp"
#include <string>
#include <boost/pool/pool_alloc.hpp>
extern "C" {
#include <limits.h>
}
#include "unit.hpp"

#define _JEBE_NO_REWIND_OPTI		0
#define _JEBE_SCAN_FROM_RIGHT		0
#define _JEBE_USE_TIMER				0
#define _JEBE_ENABLE_MAXMATCH		0

#define _JEBE_BUFF_UNIT				1024
#define _JEBE_SESS_RBUF_UNIT		1024
#define _JEBE_SESS_SBUF_UNIT		_JEBE_SESS_RBUF_UNIT

#define _JEBE_SESS_POOL_INC_STEP	32
#define _JEBE_SESS_POOL_MAX_SIZE	1024

#define _JEBE_HTTP_LINE_SEP 				"\r\n"
#define _JEBE_HTTP_SEP 						_JEBE_HTTP_LINE_SEP _JEBE_HTTP_LINE_SEP

namespace jebe {
namespace cws {

// pooled string.
typedef boost::fast_pool_allocator<staging::CSUnit<1>, boost::default_user_allocator_new_delete,
		boost::details::pool::default_mutex,
		_JEBE_SESS_POOL_INC_STEP * _JEBE_SESS_RBUF_UNIT,
		_JEBE_SESS_POOL_MAX_SIZE * _JEBE_SESS_RBUF_UNIT> RecvBuffAlloc;

// the buffer-descripter which hold a pair of (data-ptr,data-size).
typedef std::pair<char* const, std::size_t> BuffDesc;

typedef unsigned char 	byte_t;
typedef uint32_t 		tsize_t;	// text-size type.
typedef uint8_t 		wsize_t;	// word-size type.
typedef uint32_t 		atimes_t;	// appear-times of word type.

}
}
