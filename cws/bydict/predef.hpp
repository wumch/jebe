
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

#define _JEBE_SESS_POOL_INC_STEP	16
#define _JEBE_SESS_POOL_MAX_SIZE	1024

#define _JEBE_RW_BUF_RATE			2

#define _JEBE_SESS_RBUF_UNIT		2048
#define _JEBE_SESS_RBUF_SIZE		1024

#define _JEBE_SESS_POOL_INC_STEP	32
#define _JEBE_SESS_POOL_MAX_SIZE	1024

namespace jebe {
namespace cws {

// pooled string.
typedef boost::fast_pool_allocator<staging::CSUnit<1>, boost::default_user_allocator_new_delete,
		boost::details::pool::default_mutex,
		_JEBE_SESS_POOL_INC_STEP * _JEBE_SESS_RBUF_UNIT,
		_JEBE_SESS_POOL_MAX_SIZE * _JEBE_SESS_RBUF_UNIT> RecvBuffAlloc;
typedef std::basic_string<char, std::char_traits<char>, RecvBuffAlloc> pstr;

// the buffer-descripter which hold a pair of (data-ptr,data-size).
typedef std::pair<char* const, std::size_t> BuffDesc;

typedef unsigned char 	byte_t;
typedef uint32_t 		tsize_t;	// text-size type.
typedef uint8_t 		wsize_t;	// word-size type.
typedef uint32_t 		atimes_t;	// appear-times of word type.

}
}
