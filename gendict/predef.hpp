
#pragma once

#define CS_DEBUG	0
#define CS_LOG_ON	0
#define CS_USE_WCS	0

#define _JEBE_WORD_MAX_LEN				7
#define _JEBE_ASCII_WORD_MAX_LEN		20
#define _JEBE_WORD_MIN_ATIMES			10
#define _JEBE_PROCESS_STEP				(2 << 20)

#include "staging.hpp"
#include <boost/static_assert.hpp>

BOOST_STATIC_ASSERT(_JEBE_WORD_MAX_LEN < 256);

namespace jebe {
namespace cws {

#if CS_LOG_ON
std::wstringstream log;
#define CS_LOG(...)  log << __VA_ARGS__
#else
#define CS_LOG(...)
#endif

}
}
