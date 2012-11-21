
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
#include <string>
#include <boost/static_assert.hpp>
extern "C" {
#include <limits.h>
}
#include "unit.hpp"
#ifdef LOG_IF
#	undef LOG_IF
#endif
#ifdef LOG
#	error "macro LOG is already defined, before glog/logging.h included."
#	undef LOG
#endif
#include <glog/logging.h>

#define _JEBE_NO_REWIND_OPTI		0
#define _JEBE_SCAN_FROM_RIGHT		0
#define _JEBE_USE_TIMER				0
#define _JEBE_ENABLE_MAXMATCH		0
#define _JEBE_ENABLE_NOMISS			1
#define _JEBE_STEP_FWD_OPTI			0

#define _JEBE_WORD_MAX_LEN			(8U*3)
#define _JEBE_WORD_MAP_HASH_BITS	19

namespace jebe {
namespace cluster {
namespace preprocess {

typedef unsigned char 	byte_t;
typedef uint32_t 		tsize_t;	// text-size type.
typedef uint8_t 		wsize_t;	// word-size type.
typedef uint32_t 		atimes_t;	// appear-times of word type.

typedef std::string		Word;

typedef uint32_t		wordid_t;
typedef uint32_t		wordnum_t;

typedef uint32_t		docid_t;
typedef uint32_t		docnum_t;

typedef double			decimal_t;
typedef decimal_t		weight_t;

} /* namespace preprocess */
} /* namespace cluster */
} /* namespace jebe */
