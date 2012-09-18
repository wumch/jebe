
#pragma once

//#define CS_DEBUG	0
//#define CS_LOG_ON	0
#define CS_USE_WCS	1

#define _JEBE_WORD_MAX_LEN				5
#define _JEBE_ASCII_WORD_MAX_LEN		20
#define _JEBE_WORD_MIN_ATIMES			10
#define _JEBE_PROCESS_STEP				(2 << 20)

#include "staging.hpp"
#ifdef __linux
#	include <limits.h>
#endif
#define	_JEBE_GB_CHAR_MAX				((USHRT_MAX) + 1)

#include <string>
#include <boost/static_assert.hpp>

#if CS_LOG_ON
#	include <sstream>
namespace jebe {
namespace cws {
static std::wstringstream log;
}
}
#	define CS_LOG(...)		log << __VA_ARGS__
#else
#	define CS_LOG(...)
#endif

namespace jebe {
namespace cws {
typedef uint32_t atimes_t;
typedef wchar_t	CharType;
BOOST_STATIC_ASSERT(sizeof(CharType) == 4);
typedef std::wstring String;

BOOST_STATIC_ASSERT(_JEBE_WORD_MAX_LEN < 256);
}
}
