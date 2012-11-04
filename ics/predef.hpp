
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

#define _JEBE_NO_REWIND_OPTI		0
#define _JEBE_SCAN_FROM_RIGHT		0
#define _JEBE_USE_TIMER				0
#define _JEBE_ENABLE_MAXMATCH		0
#define _JEBE_STEP_FWD_OPTI			1

#define _JEBE_WORD_MAX_LEN			(8U*3)

namespace jebe {
namespace ics {

typedef unsigned char 	byte_t;
typedef uint32_t 		tsize_t;	// text-size type.
typedef uint8_t 		wsize_t;	// word-size type.
typedef uint32_t 		atimes_t;	// appear-times of word type.

typedef std::string		Word;
typedef double			weight_t;

class WordPOS
{
public:
	char main;
	char sub;

public:
	WordPOS()
		: main(0), sub(0)
	{}

	WordPOS(int nHandle)
		: main(nHandle >> 8), sub(nHandle & 0xff)
	{}

	WordPOS(char main_, char sub_)
		: main(main_), sub(sub_)
	{}
};

}
}
