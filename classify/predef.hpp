
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

#define BOOST_EXCEPTION_DIABLE		// no guy can throw exceptions to me!

#include "meta.hpp"
#include <string>
#include <boost/static_assert.hpp>
extern "C" {
#include <limits.h>
}
#ifdef LOG_IF
#	undef LOG_IF
#endif
#ifdef LOG
#	error "macro LOG is already defined, before glog/logging.h being included."
#	undef LOG
#endif
#include <glog/logging.h>

namespace jebe {
namespace classify {

typedef double			decimal_t;

typedef uint32_t		vid_t;		// vector-id type.
typedef uint32_t		vnum_t;		// vector-count type.

typedef uint32_t		fid_t;		// feature-id type. int-traits is assumed.
typedef uint32_t		fnum_t;		// feature-count type.
typedef decimal_t		fval_t;		// feature-value type. real-number is assumed.

typedef uint32_t		clsid_t;	// cluster-id type.
typedef uint32_t		clsnum_t;	// cluster-count type.

enum Action {
	wrong = 0,
	tell_config = 1,
	tell_total = 2,
	send_doc = 10,
	collected = 11,
	calculated = 12,
	thats_all = 3,
};

} /* namespace cluster */
} /* namespace jebe */
